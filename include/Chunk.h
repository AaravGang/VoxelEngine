#pragma once
#include "Mesh.h"
#include <array>

class Chunk {
public:
    static const int CHUNK_SIZE = 64;

    // Force 64-byte alignment to prevent False Sharing across CPU cores
    alignas(64) std::array<uint8_t, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;

    Mesh mesh;

    Chunk();
    uint8_t GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, uint8_t type);

    void GenerateMesh();
};