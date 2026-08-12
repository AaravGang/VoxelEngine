#pragma once
#include "Mesh.h"
#include <array>

class Chunk {
public:
    static const int CHUNK_SIZE = 64;
    std::array<uint8_t, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;

    // The chunk now directly owns its mesh
    Mesh mesh;

    Chunk();
    uint8_t GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, uint8_t type);

    void GenerateMesh();
};