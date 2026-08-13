#include "MemoryArena.h"

MemoryArena GlobalMeshArena(250);

MemoryArena::MemoryArena(size_t initialCapacity) {
    pool.reserve(initialCapacity);
    for (size_t i = 0; i < initialCapacity; ++i) {
        pool.push_back(std::make_unique<MeshBuffer>());
    }
}

std::unique_ptr<MeshBuffer> MemoryArena::Allocate() {
    std::lock_guard<std::mutex> lock(arenaMutex);
    if (pool.empty()) {
        return std::make_unique<MeshBuffer>();
    }
    auto buffer = std::move(pool.back());
    pool.pop_back();
    buffer->count = 0;
    return buffer;
}

void MemoryArena::Free(std::unique_ptr<MeshBuffer> buffer) {
    std::lock_guard<std::mutex> lock(arenaMutex);
    pool.push_back(std::move(buffer));
}