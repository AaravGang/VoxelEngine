#pragma once
#include <cstdint>
#include <glm/glm.hpp>

enum class BlockType : uint8_t {
    Air = 0,
    Grass,
    Dirt,
    Stone,
    Sand,
    ForestGrass,
    Mycelium,
    Snow,
    Wood,
    Leaves 
};
inline glm::vec3 GetBlockColor(BlockType type) {
    switch (type) {
    case BlockType::Grass:
        return glm::vec3(0.35f, 0.75f, 0.3f); // Bright Plains Grass
    case BlockType::ForestGrass:
        return glm::vec3(0.15f, 0.5f, 0.15f); // Deep Forest Green
    case BlockType::Mycelium:
        return glm::vec3(0.6f, 0.4f, 0.7f); // Purple/Pinkish
    case BlockType::Dirt:
        return glm::vec3(0.5f, 0.3f, 0.1f);
    case BlockType::Stone:
        return glm::vec3(0.5f, 0.5f, 0.5f);
    case BlockType::Snow:
        return glm::vec3(0.9f, 0.9f, 1.0f);
    case BlockType::Sand:
        return glm::vec3(0.8f, 0.8f, 0.4f);
    case BlockType::Wood:
        return glm::vec3(0.4f, 0.25f, 0.1f);
    case BlockType::Leaves:
        return glm::vec3(0.1f, 0.6f, 0.1f);
    default:
        return glm::vec3(1.0f, 1.0f, 1.0f);
    }
}