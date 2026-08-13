#pragma once
#include <array>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

// Increased to 500,000 to easily accommodate complex forests in 64x64x64 chunks.
// 500,000 floats = ~2 MB per buffer.
constexpr size_t MAX_MESH_FLOATS = 500000;

struct MeshBuffer {
    std::array<float, MAX_MESH_FLOATS> data;
    size_t count = 0;
};

class MemoryArena {
private:
    std::vector<std::unique_ptr<MeshBuffer>> pool;
    std::mutex arenaMutex;

public:
    // With 2MB buffers, an initial capacity of 250 chunks takes exactly 500MB of RAM.
    MemoryArena(size_t initialCapacity = 250);

    std::unique_ptr<MeshBuffer> Allocate();
    void Free(std::unique_ptr<MeshBuffer> buffer);
};

extern MemoryArena GlobalMeshArena;