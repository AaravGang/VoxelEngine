#include "WorldGenerator.h"
#include "Chunk.h"
#include "Block.h"
#include "TerrainConfig.h"
#include <algorithm>
#include <cmath>

uint32_t WorldGenerator::getHash(int x, int z) {
    uint32_t hash = static_cast<uint32_t>(x) * 374761393U + static_cast<uint32_t>(z) * 668265263U;
    hash = (hash ^ (hash >> 13)) * 1274126177U;
    return hash ^ (hash >> 16);
}

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

    mushroomNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    mushroomNoise.SetFrequency(TerrainConfig::MushroomFrequency);

    mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    mountainNoise.SetFrequency(TerrainConfig::MountainFrequency);
    mountainNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    mountainNoise.SetFractalOctaves(5);
}

void WorldGenerator::Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ) {
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

        float mNoise = (mountainNoise.GetNoise(x, z) + 1.0f) * 0.5f;
        return (mNoise * mNoise) * TerrainConfig::MountainAmplitude
            + TerrainConfig::MountainBaseHeight;
    };

    // ==========================================
    // PASS 1: TERRAIN GENERATION
    // ==========================================
    for (int z = 0; z < Chunk::CHUNK_SIZE; ++z) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; ++x) {

            float worldX = static_cast<float>(chunkWorldX * Chunk::CHUNK_SIZE + x);
            float worldZ = static_cast<float>(chunkWorldZ * Chunk::CHUNK_SIZE + z);

            float climate = biomeNoise.GetNoise(worldX, worldZ);
            climate = std::clamp(climate, -0.999f, 0.999f);

            float biomeMap = (climate + 1.0f) * 2.0f;
            int biomeA = static_cast<int>(biomeMap);
            int biomeB = biomeA + 1;

            float blend = biomeMap - static_cast<float>(biomeA);
            blend = blend * blend * (3.0f - 2.0f * blend);

            float heightA = getBiomeHeight(biomeA, worldX, worldZ);
            float heightB = getBiomeHeight(biomeB, worldX, worldZ);
            int height = static_cast<int>(heightA + blend * (heightB - heightA));

            int activeBiome = (blend < 0.5f) ? biomeA : biomeB;

            // BREAKING FEATURE CORRELATION:
            // We use plainsNoise (which is otherwise unused here) to perturb the snow line
            // organically.
            float snowVarianceNoise
                = (plainsNoise.GetNoise(worldX * 2.0f, worldZ * 2.0f) + 1.0f) * 0.5f;
            float localSnowLine = TerrainConfig::SnowLineBase
                + ((snowVarianceNoise - 0.5f) * 2.0f * TerrainConfig::SnowLineVariance);

            for (int y = 0; y < Chunk::CHUNK_SIZE; ++y) {
                if (y > height) {
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Air));
                } else {
                    if (activeBiome == 0) {
                        if (y > height - 3)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Sand));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 1) {
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Grass));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 2) {
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::ForestGrass));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 3) {
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Mycelium));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 4) {
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

    // ==========================================
    // PASS 2: TREE GENERATION (Decorators)
    // ==========================================
    for (int z = 2; z < Chunk::CHUNK_SIZE - 2; ++z) {
        for (int x = 2; x < Chunk::CHUNK_SIZE - 2; ++x) {

            float worldX = static_cast<float>(chunkWorldX * Chunk::CHUNK_SIZE + x);
            float worldZ = static_cast<float>(chunkWorldZ * Chunk::CHUNK_SIZE + z);

            float climate = biomeNoise.GetNoise(worldX, worldZ);
            climate = std::clamp(climate, -0.999f, 0.999f);
            float biomeMap = (climate + 1.0f) * 2.0f;
            int biomeA = static_cast<int>(biomeMap);
            int biomeB = biomeA + 1;
            float blend = biomeMap - static_cast<float>(biomeA);
            blend = blend * blend * (3.0f - 2.0f * blend);
            int activeBiome = (blend < 0.5f) ? biomeA : biomeB;

            if (activeBiome == 2) {
                uint32_t hash = getHash(static_cast<int>(worldX), static_cast<int>(worldZ));

                if (hash % 100 < 3) {

                    // PREVENTING STATE MUTATION: Check exactly what block the raycast hits
                    int surfaceY = 0;
                    uint8_t surfaceBlock = static_cast<uint8_t>(BlockType::Air);

                    for (int y = Chunk::CHUNK_SIZE - 1; y >= 0; --y) {
                        uint8_t block = chunk.GetBlock(x, y, z);
                        if (block != static_cast<uint8_t>(BlockType::Air)) {
                            surfaceY = y;
                            surfaceBlock = block;
                            break;
                        }
                    }

                    // Only build if the tree is planted on solid ForestGrass or Dirt (ignores
                    // Leaves/Wood)
                    if (surfaceY > 0 && surfaceY < Chunk::CHUNK_SIZE - 10
                        && (surfaceBlock == static_cast<uint8_t>(BlockType::ForestGrass)
                            || surfaceBlock == static_cast<uint8_t>(BlockType::Dirt))) {

                        for (int i = 1; i <= 4; ++i) {
                            chunk.SetBlock(x, surfaceY + i, z,
                                           static_cast<uint8_t>(BlockType::Wood));
                        }

                        for (int ly = surfaceY + 3; ly <= surfaceY + 5; ++ly) {
                            for (int lx = x - 2; lx <= x + 2; ++lx) {
                                for (int lz = z - 2; lz <= z + 2; ++lz) {
                                    if (std::abs(lx - x) == 2 && std::abs(lz - z) == 2
                                        && ly == surfaceY + 5)
                                        continue;

                                    if (chunk.GetBlock(lx, ly, lz)
                                        == static_cast<uint8_t>(BlockType::Air)) {
                                        chunk.SetBlock(lx, ly, lz,
                                                       static_cast<uint8_t>(BlockType::Leaves));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}