#include "Chunk.h"
#include "Block.h"
#include <glm/glm.hpp>
#include <vector>

Chunk::Chunk() {
    blocks.fill(0);
}

uint8_t Chunk::GetBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
        return 0;
    return blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

void Chunk::SetBlock(int x, int y, int z, uint8_t type) {
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
        blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = type;
    }
}

void Chunk::GenerateMesh() {
    mesh.vertices.clear();

    // Helper lambda to push 6 vertices for a generated quad
    auto addQuad = [&](glm::vec3 bottomLeft, glm::vec3 topLeft, glm::vec3 topRight,
                       glm::vec3 bottomRight, glm::vec3 color, float shading) {
        float r = color.r * shading;
        float g = color.g * shading;
        float b = color.b * shading;

        // Triangle 1
        mesh.vertices.insert(mesh.vertices.end(),
                             { bottomLeft.x, bottomLeft.y, bottomLeft.z, r, g, b });
        mesh.vertices.insert(mesh.vertices.end(), { topRight.x, topRight.y, topRight.z, r, g, b });
        mesh.vertices.insert(mesh.vertices.end(), { topLeft.x, topLeft.y, topLeft.z, r, g, b });

        // Triangle 2
        mesh.vertices.insert(mesh.vertices.end(),
                             { bottomLeft.x, bottomLeft.y, bottomLeft.z, r, g, b });
        mesh.vertices.insert(mesh.vertices.end(),
                             { bottomRight.x, bottomRight.y, bottomRight.z, r, g, b });
        mesh.vertices.insert(mesh.vertices.end(), { topRight.x, topRight.y, topRight.z, r, g, b });
    };

    // Sweep over all 3 axes (0=X, 1=Y, 2=Z)
    for (int axis = 0; axis < 3; ++axis) {
        int u = (axis + 1) % 3; // The horizontal axis of our 2D slice
        int v = (axis + 2) % 3; // The vertical axis of our 2D slice

        int x[3] = { 0, 0, 0 };
        int q[3] = { 0, 0, 0 };
        q[axis] = 1; // Direction vector for checking adjacency

        // A 2D mask tracking visible faces and their block types
        std::vector<uint8_t> mask(CHUNK_SIZE * CHUNK_SIZE);

        // Check both directions (positive and negative faces)
        for (bool backFace = false; backFace != true; backFace = !backFace) {

            // Iterate through the slices of the chunk
            for (x[axis] = -1; x[axis] < CHUNK_SIZE;) {

                // 1. BUILD THE MASK
                int n = 0;
                for (x[v] = 0; x[v] < CHUNK_SIZE; ++x[v]) {
                    for (x[u] = 0; x[u] < CHUNK_SIZE; ++x[u]) {
                        // Check the block on the current slice and the adjacent slice
                        uint8_t blockCurrent = (x[axis] >= 0) ? GetBlock(x[0], x[1], x[2]) : 0;
                        uint8_t blockNext = (x[axis] < CHUNK_SIZE - 1)
                            ? GetBlock(x[0] + q[0], x[1] + q[1], x[2] + q[2])
                            : 0;

                        // If one is solid and the other is air, a face is visible
                        bool visible = (blockCurrent != 0) != (blockNext != 0);

                        if (visible) {
                            // Store the block type that is generating the face
                            mask[n++] = backFace ? blockNext : blockCurrent;
                        } else {
                            mask[n++] = 0;
                        }
                    }
                }

                ++x[axis];
                n = 0;

                // 2. GREEDY SWEEP ACROSS THE MASK
                for (int j = 0; j < CHUNK_SIZE; ++j) {
                    for (int i = 0; i < CHUNK_SIZE;) {
                        uint8_t currentType = mask[n];
                        if (currentType != 0) {
                            int width, height;

                            // Greedily expand the width (u)
                            for (width = 1;
                                 i + width < CHUNK_SIZE && mask[n + width] == currentType;
                                 ++width) { }

                            // Greedily expand the height (v)
                            bool done = false;
                            for (height = 1; j + height < CHUNK_SIZE; ++height) {
                                for (int k = 0; k < width; ++k) {
                                    if (mask[n + k + height * CHUNK_SIZE] != currentType) {
                                        done = true;
                                        break;
                                    }
                                }
                                if (done)
                                    break;
                            }

                            // Erase the merged quad from the mask so we don't draw it again
                            for (int l = 0; l < height; ++l) {
                                for (int k = 0; k < width; ++k) {
                                    mask[n + k + l * CHUNK_SIZE] = 0;
                                }
                            }

                            // 3. GENERATE THE VERTICES
                            x[u] = i;
                            x[v] = j;

                            int du[3] = { 0, 0, 0 };
                            du[u] = width;
                            int dv[3] = { 0, 0, 0 };
                            dv[v] = height;

                            glm::vec3 bottomLeft = glm::vec3(x[0], x[1], x[2]);
                            glm::vec3 bottomRight
                                = glm::vec3(x[0] + du[0], x[1] + du[1], x[2] + du[2]);
                            glm::vec3 topLeft = glm::vec3(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]);
                            glm::vec3 topRight = glm::vec3(
                                x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2]);

                            // Retrieve color based on the unified block type
                            glm::vec3 baseColor
                                = GetBlockColor(static_cast<BlockType>(currentType));

                            // Apply directional shading based on the axis to restore depth
                            float shading = 1.0f;
                            if (axis == 0)
                                shading = 0.85f; // X sides
                            if (axis == 1)
                                shading = backFace ? 0.4f : 1.0f; // Y top (1.0) and bottom (0.4)
                            if (axis == 2)
                                shading = 0.7f; // Z sides

                            // Ensure correct winding order so faces aren't culled incorrectly by
                            // OpenGL
                            if (backFace) {
                                addQuad(bottomLeft, bottomRight, topRight, topLeft, baseColor,
                                        shading);
                            } else {
                                addQuad(bottomLeft, topLeft, topRight, bottomRight, baseColor,
                                        shading);
                            }

                            i += width;
                            n += width;
                        } else {
                            i++;
                            n++;
                        }
                    }
                }
            }
            if (backFace)
                break; // Break out after checking the negative direction
        }
    }
}