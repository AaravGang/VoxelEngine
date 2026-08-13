#pragma once
#include "Mesh.h"
#include <array>
#include <cstdint>

// 1. Force the entire chunk to start precisely at the beginning of a physical cache line
class alignas(64) Chunk {
public:
    static constexpr int CHUNK_SIZE = 64;
    static constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

private:
    // Ensure you are using a flat 1D array for data locality!
    std::array<uint8_t, CHUNK_VOLUME> blocks;

public:
    Mesh mesh;

    Chunk();
    
    // O(1) mathematical index lookup
    inline void SetBlock(int x, int y, int z, uint8_t type) {
        blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = type;
    }

    inline uint8_t GetBlock(int x, int y, int z) const {
        return blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
    }

    void GenerateMesh();
};