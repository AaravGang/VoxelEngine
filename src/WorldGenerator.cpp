#include "WorldGenerator.h"
#include "Block.h"
#include "Chunk.h"
#include "TerrainConfig.h"
#include <algorithm> // Required for std::clamp

WorldGenerator::WorldGenerator() {
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    biomeNoise.SetFrequency(TerrainConfig::ClimateFrequency);

    desertNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    desertNoise.SetFrequency(TerrainConfig::DesertFrequency);
    desertNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    desertNoise.SetFractalOctaves(2);

    plainsNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    plainsNoise.SetFrequency(TerrainConfig::PlainsFrequency);

    forestNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    forestNoise.SetFrequency(TerrainConfig::ForestFrequency);
    forestNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    forestNoise.SetFractalOctaves(3);

    mushroomNoise.SetNoiseType(
        FastNoiseLite::NoiseType_Cellular); // Cellular makes weird, bulbous hills
    mushroomNoise.SetFrequency(TerrainConfig::MushroomFrequency);

    mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    mountainNoise.SetFrequency(TerrainConfig::MountainFrequency);
    mountainNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    mountainNoise.SetFractalOctaves(5);
}

void WorldGenerator::Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) {
    // Helper lambda to fetch height based on the biome index
    auto getBiomeHeight = [&](int biome, float x, float z) -> float {
        if (biome == 0)
            return ((desertNoise.GetNoise(x, z) + 1.0f) * 0.5f) * TerrainConfig::DesertAmplitude
                + TerrainConfig::DesertBaseHeight;
        if (biome == 1)
            return ((plainsNoise.GetNoise(x, z) + 1.0f) * 0.5f) * TerrainConfig::PlainsAmplitude
                + TerrainConfig::PlainsBaseHeight;
        if (biome == 2)
            return ((forestNoise.GetNoise(x, z) + 1.0f) * 0.5f) * TerrainConfig::ForestAmplitude
                + TerrainConfig::ForestBaseHeight;
        if (biome == 3)
            return ((mushroomNoise.GetNoise(x, z) + 1.0f) * 0.5f) * TerrainConfig::MushroomAmplitude
                + TerrainConfig::MushroomBaseHeight;

        // Default to Mountain (4)
        float mNoise = (mountainNoise.GetNoise(x, z) + 1.0f) * 0.5f;
        return (mNoise * mNoise) * TerrainConfig::MountainAmplitude
            + TerrainConfig::MountainBaseHeight;
    };

    for (int z = 0; z < Chunk::CHUNK_SIZE; ++z) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; ++x) {

            float worldX = static_cast<float>(chunkWorldX * Chunk::CHUNK_SIZE + x);
            float worldZ = static_cast<float>(chunkWorldZ * Chunk::CHUNK_SIZE + z);

            // 1. Get Climate and clamp it safely between -0.999 and 0.999
            float climate = biomeNoise.GetNoise(worldX, worldZ);
            climate = std::clamp(climate, -0.999f, 0.999f);

            // 2. Map climate to a 0.0 -> 4.0 scale
            float biomeMap = (climate + 1.0f) * 2.0f;

            // 3. Find our two neighboring biomes
            int biomeA = static_cast<int>(biomeMap);
            int biomeB = biomeA + 1;

            // 4. Calculate exactly how far we are between them (0.0 to 1.0)
            float blend = biomeMap - static_cast<float>(biomeA);
            blend = blend * blend * (3.0f - 2.0f * blend); // Smoothstep curve

            // 5. LERP the heights
            float heightA = getBiomeHeight(biomeA, worldX, worldZ);
            float heightB = getBiomeHeight(biomeB, worldX, worldZ);
            int height = static_cast<int>(heightA + blend * (heightB - heightA));

            // 6. Determine which biome is dominant here for surface block colors
            int activeBiome = (blend < 0.5f) ? biomeA : biomeB;

            float mNoise = (mountainNoise.GetNoise(worldX, worldZ) + 1.0f) * 0.5f;
            float localSnowLine = TerrainConfig::SnowLineBase
                + ((mNoise - 0.5f) * 2.0f * TerrainConfig::SnowLineVariance);

            // 7. Place the blocks!
            for (int y = 0; y < Chunk::CHUNK_SIZE; ++y) {
                if (y > height) {
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Air));
                } else {
                    if (activeBiome == 0) { // Desert
                        if (y > height - 3)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Sand));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 1) { // Plains
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Grass));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 2) { // Forest
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::ForestGrass));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 3) { // Mushroom
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Mycelium));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 4) { // Mountain
                        if (y > localSnowLine)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Snow));
                        else if (y > height - 2)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    }
                }
            }
        }
    }
}