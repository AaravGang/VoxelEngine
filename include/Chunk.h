#pragma once
#include "Mesh.h"
#include <array>

class Chunk {
public:
    static const int CHUNK_SIZE = 32;

    // Flat 1D array representing a 32x32x32 grid
    std::array<uint8_t, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;

    Chunk();

    // Helper to get block type, handling out-of-bounds checks
    uint8_t GetBlock(int x, int y, int z) const;

    // Helper to set block type
    void SetBlock(int x, int y, int z, uint8_t type);

    // Iterates through the array and generates the 3D geometry
    void GenerateMesh(Mesh& mesh);
};