#include "WorldGenerator.h"
#include "Block.h"

void WorldGenerator::Generate(Chunk& chunk, int chunkWorldX, int chunkWorldZ,
                              const std::vector<float>& gpuHeightMap) {
    // We no longer need Pass 1 (SIMD Math). The GPU already did it!

    // ==========================================
    // PASS 2: TERRAIN PLACEMENT (Using GPU RGBA Data)
    // ==========================================
    for (int z = 0; z < Chunk::CHUNK_SIZE; ++z) {
        for (int y = 0; y < Chunk::CHUNK_SIZE; ++y) {
            for (int x = 0; x < Chunk::CHUNK_SIZE; ++x) {

                // Calculate base index for this block's X/Z coordinate
                int baseIndex = (x + z * Chunk::CHUNK_SIZE) * 4;

                // Extract the data from the GPU's color channels
                int height = static_cast<int>(gpuHeightMap[baseIndex + 0]);
                int activeBiome = static_cast<int>(gpuHeightMap[baseIndex + 1]);
                float localSnowLine = gpuHeightMap[baseIndex + 2];

                if (y > height) {
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Air));
                }
                // THE FIX: Only generate snow if we are in Biome 4 (Winter/Tundra)
                else if (activeBiome == 4 && y >= localSnowLine) {
                    chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Snow));
                } else {
                    // Normal biome logic for everything else!
                    if (activeBiome == 0) {
                        // Desert
                        if (y > height - 3)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Sand));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 1) {
                        // Plains
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Grass));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 2) {
                        // Forest
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::ForestGrass));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 3) {
                        // Mushroom
                        if (y > height - 1)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Mycelium));
                        else if (y > height - 4)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    } else if (activeBiome == 4) {
                        // Winter (Below the snowline)
                        if (y > height - 2)
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                        else
                            chunk.SetBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                    }
                }
            }
        }
    }

    // ==========================================
    // PASS 3: STRUCTURE GENERATION (TREES)
    // ==========================================
    for (int z = 5; z < Chunk::CHUNK_SIZE - 5; ++z) {
        for (int x = 5; x < Chunk::CHUNK_SIZE - 5; ++x) {

            int i = (x + z * Chunk::CHUNK_SIZE) * 4;
            int height = static_cast<int>(gpuHeightMap[i]);
            int activeBiome = static_cast<int>(gpuHeightMap[i + 1]);

            // Only grow trees on Grass or ForestGrass, and ensure we have vertical space
            if ((activeBiome == 1 || activeBiome == 2) && height > 0
                && height < Chunk::CHUNK_SIZE - 6) {

                // Deterministic Spatial Hash: Always gives the exact same result for a given X,Z
                // coordinate
                int absoluteX = chunkWorldX * Chunk::CHUNK_SIZE + x;
                int absoluteZ = chunkWorldZ * Chunk::CHUNK_SIZE + z;
                uint32_t hash = (absoluteX * 73856093) ^ (absoluteZ * 19349663);

                // 1.5% chance to spawn a tree on this specific block
                if (hash % 1000 < 15) {

                    // Build Trunk
                    for (int ty = 1; ty <= 4; ++ty) {
                        chunk.SetBlock(x, height + ty, z, static_cast<uint8_t>(BlockType::Wood));
                    }

                    // Build Leaves (Simple Cube/Diamond around top of trunk)
                    for (int ly = 3; ly <= 5; ++ly) {
                        for (int lx = -2; lx <= 2; ++lx) {
                            for (int lz = -2; lz <= 2; ++lz) {
                                // Cut the corners to make it look organic
                                if (abs(lx) == 2 && abs(lz) == 2 && ly == 5)
                                    continue;
                                if (abs(lx) == 2 && abs(lz) == 2 && ly == 3)
                                    continue;

                                // Don't overwrite the trunk
                                if (lx == 0 && lz == 0 && ly < 5)
                                    continue;

                                chunk.SetBlock(x + lx, height + ly, z + lz,
                                               static_cast<uint8_t>(BlockType::Leaves));
                            }
                        }
                    }
                }
            }
        }
    }
}