#pragma once
#include "FastNoiseLite.h"
#include "ITerrainGenerator.h"

// Generates flat, rolling sand dunes
class DesertGenerator : public ITerrainGenerator {
private:
    FastNoiseLite noise;

public:
    DesertGenerator();
    void Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) override;
};

// Generates high-frequency jagged peaks
class MountainGenerator : public ITerrainGenerator {
private:
    FastNoiseLite noise;

public:
    MountainGenerator();
    void Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) override;
};