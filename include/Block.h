#pragma once
#include <cstdint>
#include <glm/glm.hpp>

enum class BlockType : uint8_t {
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
    Snow = 4,
    Sand = 5,
    Water = 6,
    ForestGrass = 7,
    Mycelium = 8
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
    default:
        return glm::vec3(1.0f, 1.0f, 1.0f);
    }
}