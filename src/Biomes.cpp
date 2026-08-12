#include "Biomes.h"
#include "Block.h"
#include "Chunk.h"

// --- Desert Biome ---
DesertGenerator::DesertGenerator() {
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.015f); // Very low frequency for wide dunes

    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(2);
}

void DesertGenerator::Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) {
    for (int z = 0; z < Chunk::CHUNK_SIZE; ++z) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; ++x) {
            float worldX = static_cast<float>(chunkWorldX * Chunk::CHUNK_SIZE + x);
            float worldZ = static_cast<float>(chunkWorldZ * Chunk::CHUNK_SIZE + z);

            float noiseVal = (noise.GetNoise(worldX, worldZ) + 1.0f) * 0.5f;
            int height = static_cast<int>(noiseVal * 20.0f) + 5;

            for (int y = 0; y < Chunk::CHUNK_SIZE; ++y) {
                if (y > height)
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Air));
                else if (y > height - 3)
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Sand));
                else
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
            }
        }
    }
}

// --- Mountain Biome ---
MountainGenerator::MountainGenerator() {
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.02f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(5);
}

void MountainGenerator::Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) {
    for (int z = 0; z < Chunk::CHUNK_SIZE; ++z) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; ++x) {
            float worldX = static_cast<float>(chunkWorldX * Chunk::CHUNK_SIZE + x);
            float worldZ = static_cast<float>(chunkWorldZ * Chunk::CHUNK_SIZE + z);

            float noiseVal = (noise.GetNoise(worldX, worldZ) + 1.0f) * 0.5f;
            noiseVal = noiseVal * noiseVal;
            int height = static_cast<int>(noiseVal * 40.0f) + 2;

            for (int y = 0; y < Chunk::CHUNK_SIZE; ++y) {
                if (y > height)
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Air));
                else if (y > 24)
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Snow));
                else if (y > height - 2)
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                else
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
            }
        }
    }
}