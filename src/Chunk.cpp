#include "Chunk.h"
#include "Block.h"
#include "MemoryArena.h"
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
    // Claim memory from the Arena in O(1) time
    mesh.buffer = GlobalMeshArena.Allocate();

    auto addQuad
        = [&](glm::vec3 bottomLeft, glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomRight,
              glm::vec3 color, float shading, bool backFace, float w, float h) {
              // Safety check: 6 vertices * 8 floats = 48
              if (mesh.buffer->count + 48 > MAX_MESH_FLOATS)
                  return;

              float r = color.r * shading;
              float g = color.g * shading;
              float b = color.b * shading;

              auto& data = mesh.buffer->data;
              size_t& idx = mesh.buffer->count;

              auto pushVertex = [&](glm::vec3 pos, float u, float v) {
                  data[idx++] = pos.x;
                  data[idx++] = pos.y;
                  data[idx++] = pos.z;
                  data[idx++] = r;
                  data[idx++] = g;
                  data[idx++] = b;
                  data[idx++] = u;
                  data[idx++] = v;
              };

              if (backFace) {
                  pushVertex(bottomLeft, 0.0f, 0.0f);
                  pushVertex(bottomRight, w, 0.0f);
                  pushVertex(topRight, w, h);
                  pushVertex(bottomLeft, 0.0f, 0.0f);
                  pushVertex(topRight, w, h);
                  pushVertex(topLeft, 0.0f, h);
              } else {
                  pushVertex(bottomLeft, 0.0f, 0.0f);
                  pushVertex(topLeft, 0.0f, h);
                  pushVertex(topRight, w, h);
                  pushVertex(bottomLeft, 0.0f, 0.0f);
                  pushVertex(topRight, w, h);
                  pushVertex(bottomRight, w, 0.0f);
              }
          };

    for (int axis = 0; axis < 3; ++axis) {
        int u = (axis + 1) % 3;
        int v = (axis + 2) % 3;
        int x[3] = { 0, 0, 0 };
        int q[3] = { 0, 0, 0 };
        q[axis] = 1;

        std::vector<uint8_t> mask(CHUNK_SIZE * CHUNK_SIZE);

        // 0 = Forward Facing (+), 1 = Backward Facing (-)
        for (int dir = 0; dir < 2; ++dir) {
            bool backFace = (dir == 1);

            for (x[axis] = -1; x[axis] < CHUNK_SIZE;) {

                // 1. BUILD THE MASK (With flawless face culling)
                int n = 0;
                for (x[v] = 0; x[v] < CHUNK_SIZE; ++x[v]) {
                    for (x[u] = 0; x[u] < CHUNK_SIZE; ++x[u]) {
                        uint8_t blockCurrent = (x[axis] >= 0) ? GetBlock(x[0], x[1], x[2]) : 0;
                        uint8_t blockNext = (x[axis] < CHUNK_SIZE - 1)
                            ? GetBlock(x[0] + q[0], x[1] + q[1], x[2] + q[2])
                            : 0;

                        bool visible = false;
                        if (backFace) {
                            visible = (blockNext != 0 && blockCurrent == 0);
                            mask[n++] = visible ? blockNext : 0;
                        } else {
                            visible = (blockCurrent != 0 && blockNext == 0);
                            mask[n++] = visible ? blockCurrent : 0;
                        }
                    }
                }

                ++x[axis];
                n = 0;

                // 2. GREEDY EXPANSION
                for (int j = 0; j < CHUNK_SIZE; ++j) {
                    for (int i = 0; i < CHUNK_SIZE;) {
                        uint8_t currentType = mask[n];
                        if (currentType != 0) {
                            int width, height;

                            for (width = 1;
                                 i + width < CHUNK_SIZE && mask[n + width] == currentType;
                                 ++width) { }

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

                            for (int l = 0; l < height; ++l) {
                                for (int k = 0; k < width; ++k) {
                                    mask[n + k + l * CHUNK_SIZE] = 0;
                                }
                            }

                            // 3. GENERATE VERTICES
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

                            glm::vec3 baseColor
                                = GetBlockColor(static_cast<BlockType>(currentType));

                            float shading = 1.0f;
                            if (axis == 0)
                                shading = 0.85f;
                            if (axis == 1)
                                shading = backFace ? 0.4f : 1.0f;
                            if (axis == 2)
                                shading = 0.7f;

                            // Pass (float)width and (float)height at the very end of the arguments!
                            addQuad(bottomLeft, topLeft, topRight, bottomRight, baseColor, shading,
                                    backFace, static_cast<float>(width),
                                    static_cast<float>(height));

                            i += width;
                            n += width;
                        } else {
                            i++;
                            n++;
                        }
                    }
                }
            }
        }
    }
}