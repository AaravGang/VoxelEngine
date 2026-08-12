#include "SimplexTerrainGenerator.h"
#include "Chunk.h"
#include <cmath>

SimplexTerrainGenerator::SimplexTerrainGenerator() {
    // Configure FastNoiseLite for 2D terrain heightmaps
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.015f); // Controls terrain feature size (smaller = larger hills)
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4); // Layered detail
}

void SimplexTerrainGenerator::Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) {
    for (int z = 0; z < Chunk::CHUNK_SIZE; ++z) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; ++x) {

            float worldX = static_cast<float>(chunkWorldX * Chunk::CHUNK_SIZE + x);
            float worldZ = static_cast<float>(chunkWorldZ * Chunk::CHUNK_SIZE + z);

            float noiseVal = noise.GetNoise(worldX, worldZ);
            float normalizedNoise = (noiseVal + 1.0f) * 0.5f;

            // Let's make the terrain taller so biomes are more obvious
            int terrainHeight = static_cast<int>(normalizedNoise * 30.0f) + 2;

            for (int y = 0; y < Chunk::CHUNK_SIZE; ++y) {
                if (y > terrainHeight) {
                    chunk.SetBlock(x, y, z, 0); // Air
                } else if (y == terrainHeight) {
                    // Surface Biomes
                    if (y < 6)
                        chunk.SetBlock(x, y, z, 5); // Sand (Lowlands)
                    else if (y > 22)
                        chunk.SetBlock(x, y, z, 4); // Snow (Peaks)
                    else
                        chunk.SetBlock(x, y, z, 1); // Grass (Midlands)
                } else if (y >= terrainHeight - 3) {
                    chunk.SetBlock(x, y, z, 2); // Dirt (Subsurface)
                } else {
                    chunk.SetBlock(x, y, z, 3); // Stone (Deep underground)
                }
            }
        }
    }
}