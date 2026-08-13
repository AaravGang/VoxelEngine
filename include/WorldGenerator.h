#pragma once
#include "Chunk.h"
#include <vector>

class WorldGenerator {
public:
    // Notice the new parameter: the pre-calculated GPU height map!
    void Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ,
                  const std::vector<float>& gpuHeightMap);
};