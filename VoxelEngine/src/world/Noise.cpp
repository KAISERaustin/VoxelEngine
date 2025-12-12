#include "world/Noise.h"

#include <cmath>
#include <algorithm>

static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float fade(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

static float hash2d_to_01(int x, int z, uint32_t seed)
{
    uint32_t h = seed;
    h ^= static_cast<uint32_t>(x) * 0x9E3779B9u;
    h ^= static_cast<uint32_t>(z) * 0x85EBCA6Bu;
    h ^= (h >> 16);
    h *= 0xC2B2AE35u;
    h ^= (h >> 16);
    return static_cast<float>(h & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
}

static float value_noise_2d(float x, float z, uint32_t seed)
{
    const int x0 = static_cast<int>(std::floor(x));
    const int z0 = static_cast<int>(std::floor(z));
    const int x1 = x0 + 1;
    const int z1 = z0 + 1;

    const float tx = x - static_cast<float>(x0);
    const float tz = z - static_cast<float>(z0);

    const float u = fade(tx);
    const float v = fade(tz);

    const float a = hash2d_to_01(x0, z0, seed);
    const float b = hash2d_to_01(x1, z0, seed);
    const float c = hash2d_to_01(x0, z1, seed);
    const float d = hash2d_to_01(x1, z1, seed);

    const float ab = lerp(a, b, u);
    const float cd = lerp(c, d, u);
    return lerp(ab, cd, v);
}

float fbm_2d(float x, float z, uint32_t seed, int octaves, float lacunarity, float gain)
{
    float amp = 1.0f;
    float freq = 1.0f;
    float sum = 0.0f;
    float norm = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        sum += amp * value_noise_2d(x * freq, z * freq, seed + static_cast<uint32_t>(i) * 1013u);
        norm += amp;
        amp *= gain;
        freq *= lacunarity;
    }

    return (norm > 0.0f) ? (sum / norm) : 0.0f;
}

int terrain_height_10_16(int gx, int gz)
{
    constexpr uint32_t SEED = 1337u;
    const float scale = 0.075f;

    const float n = fbm_2d(gx * scale, gz * scale, SEED, 4, 2.0f, 0.5f);
    const int h = 10 + static_cast<int>(std::floor(n * 7.0f));
    return std::clamp(h, 10, 16);
}
