#pragma once
#include "FastNoiseLite.h"
#include "ITerrainGenerator.h"

class WorldGenerator : public ITerrainGenerator {
private:
    FastNoiseLite biomeNoise;
    FastNoiseLite desertNoise;
    FastNoiseLite plainsNoise;
    FastNoiseLite forestNoise;
    FastNoiseLite mushroomNoise;
    FastNoiseLite mountainNoise;

public:
    WorldGenerator();
    void Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) override;
};