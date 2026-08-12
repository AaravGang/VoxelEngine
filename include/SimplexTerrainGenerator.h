#pragma once
#include "FastNoiseLite.h"
#include "ITerrainGenerator.h"

class SimplexTerrainGenerator : public ITerrainGenerator {
private:
    FastNoiseLite noise;

public:
    SimplexTerrainGenerator();

    void Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) override;
};