#pragma once

// Forward declaration
class Chunk;

class ITerrainGenerator {
public:
    virtual ~ITerrainGenerator() = default;

    // Interface method to generate voxel terrain inside a chunk
    virtual void Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) = 0;
};