#pragma once
#include "FastNoiseLite.h"
#include "ITerrainGenerator.h"
#include <cstdint> // Needed for uint32_t

class WorldGenerator : public ITerrainGenerator {
private:
    FastNoiseLite biomeNoise;
    FastNoiseLite desertNoise;
    FastNoiseLite plainsNoise;
    FastNoiseLite forestNoise;
    FastNoiseLite mushroomNoise;
    FastNoiseLite mountainNoise;

    // A non-linear bit-scrambling hash for organic scattering
    uint32_t getHash(int x, int z);

public:
    WorldGenerator();
    void Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) override;
};