#pragma once

#include <cstdint>

float fbm_2d(float x, float z, uint32_t seed, int octaves, float lacunarity, float gain);
int terrain_height_10_16(int gx, int gz);
