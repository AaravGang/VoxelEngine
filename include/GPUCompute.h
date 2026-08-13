#pragma once
#include "Shader.h"
#include <vector>

class GPUCompute {
private:
    unsigned int FBO, TextureID;
    unsigned int quadVAO, quadVBO;
    Shader computeShader;

public:
    GPUCompute();
    ~GPUCompute();

    // Tells the GPU to run the math for a specific chunk, and returns a flat array of 4,096 heights
    std::vector<float> GenerateHeightMap(int chunkWorldX, int chunkWorldZ);
};