#pragma once

namespace TerrainConfig {
// Zoomed out climate map
constexpr float ClimateFrequency = 0.003f;

// 0: Desert (Rolling dunes)
constexpr float DesertFrequency = 0.015f;
constexpr float DesertAmplitude = 20.0f;
constexpr float DesertBaseHeight = 5.0f;

// 1: Plains (Very flat, wide)
constexpr float PlainsFrequency = 0.008f;
constexpr float PlainsAmplitude = 4.0f;
constexpr float PlainsBaseHeight = 8.0f;

// 2: Forest (Bumpy and hilly)
constexpr float ForestFrequency = 0.02f;
constexpr float ForestAmplitude = 12.0f;
constexpr float ForestBaseHeight = 10.0f;

// 3: Mushroom (Weird, steep, isolated hills)
constexpr float MushroomFrequency = 0.03f;
constexpr float MushroomAmplitude = 25.0f;
constexpr float MushroomBaseHeight = 6.0f;

// 4: Mountain (Aggressive peaks)
constexpr float MountainFrequency = 0.02f;
constexpr float MountainAmplitude = 45.0f;
constexpr float MountainBaseHeight = 2.0f;

constexpr float SnowLineBase = 30.0f;
constexpr float SnowLineVariance = 6.0f;
}