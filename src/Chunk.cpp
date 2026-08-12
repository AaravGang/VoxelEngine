#include "Chunk.h"

Chunk::Chunk() {
    // Fill the chunk with blocks for testing (1 = solid, 0 = air)
    // We will only fill the bottom half so it looks like ground
    blocks.fill(0);
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE / 2; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                SetBlock(x, y, z, 1);
            }
        }
    }
}

uint8_t Chunk::GetBlock(int x, int y, int z) const {
    // If we look outside the chunk bounds, treat it as air (0)
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return 0;
    }
    return blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

void Chunk::SetBlock(int x, int y, int z, uint8_t type) {
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
        blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = type;
    }
}

void Chunk::GenerateMesh(Mesh& mesh) {
    mesh.vertices.clear();

    // Naive Meshing: Loop through every single voxel
    for (int z = 0; z < CHUNK_SIZE; ++z) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {

                // If it is air, skip it
                if (GetBlock(x, y, z) == 0)
                    continue;

                // Explicitly cast our integers to floats to satisfy C++ list-initialization rules
                float fx = static_cast<float>(x);
                float fy = static_cast<float>(y);
                float fz = static_cast<float>(z);

                // Check Front Face (+Z)
                if (GetBlock(x, y, z + 1) == 0) {
                    float face[]
                        = { fx,        fy,        fz + 1.0f, fx + 1.0f, fy,        fz + 1.0f,
                            fx + 1.0f, fy + 1.0f, fz + 1.0f, fx + 1.0f, fy + 1.0f, fz + 1.0f,
                            fx,        fy + 1.0f, fz + 1.0f, fx,        fy,        fz + 1.0f };
                    mesh.vertices.insert(mesh.vertices.end(), std::begin(face), std::end(face));
                }
                // Check Back Face (-Z)
                if (GetBlock(x, y, z - 1) == 0) {
                    float face[] = { fx + 1.0f, fy,        fz, fx,        fy,        fz,
                                     fx,        fy + 1.0f, fz, fx,        fy + 1.0f, fz,
                                     fx + 1.0f, fy + 1.0f, fz, fx + 1.0f, fy,        fz };
                    mesh.vertices.insert(mesh.vertices.end(), std::begin(face), std::end(face));
                }
                // Check Left Face (-X)
                if (GetBlock(x - 1, y, z) == 0) {
                    float face[] = { fx, fy,        fz,        fx, fy,        fz + 1.0f,
                                     fx, fy + 1.0f, fz + 1.0f, fx, fy + 1.0f, fz + 1.0f,
                                     fx, fy + 1.0f, fz,        fx, fy,        fz };
                    mesh.vertices.insert(mesh.vertices.end(), std::begin(face), std::end(face));
                }
                // Check Right Face (+X)
                if (GetBlock(x + 1, y, z) == 0) {
                    float face[]
                        = { fx + 1.0f, fy,        fz + 1.0f, fx + 1.0f, fy,        fz,
                            fx + 1.0f, fy + 1.0f, fz,        fx + 1.0f, fy + 1.0f, fz,
                            fx + 1.0f, fy + 1.0f, fz + 1.0f, fx + 1.0f, fy,        fz + 1.0f };
                    mesh.vertices.insert(mesh.vertices.end(), std::begin(face), std::end(face));
                }
                // Check Top Face (+Y)
                if (GetBlock(x, y + 1, z) == 0) {
                    float face[]
                        = { fx,        fy + 1.0f, fz + 1.0f, fx + 1.0f, fy + 1.0f, fz + 1.0f,
                            fx + 1.0f, fy + 1.0f, fz,        fx + 1.0f, fy + 1.0f, fz,
                            fx,        fy + 1.0f, fz,        fx,        fy + 1.0f, fz + 1.0f };
                    mesh.vertices.insert(mesh.vertices.end(), std::begin(face), std::end(face));
                }
                // Check Bottom Face (-Y)
                if (GetBlock(x, y - 1, z) == 0) {
                    float face[] = { fx,        fy, fz,        fx + 1.0f, fy, fz,
                                     fx + 1.0f, fy, fz + 1.0f, fx + 1.0f, fy, fz + 1.0f,
                                     fx,        fy, fz + 1.0f, fx,        fy, fz };
                    mesh.vertices.insert(mesh.vertices.end(), std::begin(face), std::end(face));
                }
            }
        }
    }
}