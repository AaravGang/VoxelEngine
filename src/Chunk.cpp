#include "Chunk.h"
#include "Block.h"
#include <glm/glm.hpp>

Chunk::Chunk() {
    // Array starts completely empty, generator will fill it
    blocks.fill(0);
}

uint8_t Chunk::GetBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return 0; // Treat out of bounds as Air
    }
    return blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

void Chunk::SetBlock(int x, int y, int z, uint8_t type) {
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
        blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = type;
    }
}

// Notice there is NO parameter inside the parentheses here!
void Chunk::GenerateMesh() {
    mesh.vertices.clear();

    auto addFace = [&](const float* positions, float r, float g, float b) {
        for (int i = 0; i < 6; ++i) {
            mesh.vertices.push_back(positions[i * 3 + 0]); // X
            mesh.vertices.push_back(positions[i * 3 + 1]); // Y
            mesh.vertices.push_back(positions[i * 3 + 2]); // Z
            mesh.vertices.push_back(r); // R
            mesh.vertices.push_back(g); // G
            mesh.vertices.push_back(b); // B
        }
    };

    for (int z = 0; z < CHUNK_SIZE; ++z) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {

                uint8_t type = GetBlock(x, y, z);
                if (type == 0)
                    continue; // Skip Air blocks

                glm::vec3 color = GetBlockColor(static_cast<BlockType>(type));
                float r = color.r, g = color.g, b = color.b;

                float fx = static_cast<float>(x);
                float fy = static_cast<float>(y);
                float fz = static_cast<float>(z);

                // +Z Face (Side - darker)
                if (GetBlock(x, y, z + 1) == 0) {
                    float face[]
                        = { fx,        fy,        fz + 1.0f, fx + 1.0f, fy,        fz + 1.0f,
                            fx + 1.0f, fy + 1.0f, fz + 1.0f, fx + 1.0f, fy + 1.0f, fz + 1.0f,
                            fx,        fy + 1.0f, fz + 1.0f, fx,        fy,        fz + 1.0f };
                    addFace(face, r * 0.7f, g * 0.7f, b * 0.7f);
                }
                // -Z Face (Side - darker)
                if (GetBlock(x, y, z - 1) == 0) {
                    float face[] = { fx + 1.0f, fy,        fz, fx,        fy,        fz,
                                     fx,        fy + 1.0f, fz, fx,        fy + 1.0f, fz,
                                     fx + 1.0f, fy + 1.0f, fz, fx + 1.0f, fy,        fz };
                    addFace(face, r * 0.7f, g * 0.7f, b * 0.7f);
                }
                // -X Face (Side - slightly lighter than Z to create contrast)
                if (GetBlock(x - 1, y, z) == 0) {
                    float face[] = { fx, fy,        fz,        fx, fy,        fz + 1.0f,
                                     fx, fy + 1.0f, fz + 1.0f, fx, fy + 1.0f, fz + 1.0f,
                                     fx, fy + 1.0f, fz,        fx, fy,        fz };
                    addFace(face, r * 0.85f, g * 0.85f, b * 0.85f);
                }
                // +X Face (Side - slightly lighter)
                if (GetBlock(x + 1, y, z) == 0) {
                    float face[]
                        = { fx + 1.0f, fy,        fz + 1.0f, fx + 1.0f, fy,        fz,
                            fx + 1.0f, fy + 1.0f, fz,        fx + 1.0f, fy + 1.0f, fz,
                            fx + 1.0f, fy + 1.0f, fz + 1.0f, fx + 1.0f, fy,        fz + 1.0f };
                    addFace(face, r * 0.85f, g * 0.85f, b * 0.85f);
                }
                // +Y Face (Top - full sunlight, no darkening)
                if (GetBlock(x, y + 1, z) == 0) {
                    float face[]
                        = { fx,        fy + 1.0f, fz + 1.0f, fx + 1.0f, fy + 1.0f, fz + 1.0f,
                            fx + 1.0f, fy + 1.0f, fz,        fx + 1.0f, fy + 1.0f, fz,
                            fx,        fy + 1.0f, fz,        fx,        fy + 1.0f, fz + 1.0f };
                    addFace(face, r * 1.0f, g * 1.0f, b * 1.0f);
                }
                // -Y Face (Bottom - heavy shadow)
                if (GetBlock(x, y - 1, z) == 0) {
                    float face[] = { fx,        fy, fz,        fx + 1.0f, fy, fz,
                                     fx + 1.0f, fy, fz + 1.0f, fx + 1.0f, fy, fz + 1.0f,
                                     fx,        fy, fz + 1.0f, fx,        fy, fz };
                    addFace(face, r * 0.4f, g * 0.4f, b * 0.4f);
                }
            }
        }
    }
}