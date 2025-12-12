#pragma once

#include "world/Chunk.h"

#include <array>

static constexpr int WORLD_CHUNKS_X = 6;
static constexpr int WORLD_CHUNKS_Y = 1;
static constexpr int WORLD_CHUNKS_Z = 6;

static constexpr int WORLD_SIZE_X = WORLD_CHUNKS_X * CHUNK_X;
static constexpr int WORLD_SIZE_Y = WORLD_CHUNKS_Y * CHUNK_Y;
static constexpr int WORLD_SIZE_Z = WORLD_CHUNKS_Z * CHUNK_Z;

struct World
{
    std::array<Chunk, WORLD_CHUNKS_X* WORLD_CHUNKS_Y* WORLD_CHUNKS_Z> chunks{};

    static constexpr int cidx(int cx, int cy, int cz)
    {
        return cx + WORLD_CHUNKS_X * (cy + WORLD_CHUNKS_Y * cz);
    }

    Chunk& chunk_at(int cx, int cy, int cz) { return chunks[cidx(cx, cy, cz)]; }
    const Chunk& chunk_at(int cx, int cy, int cz) const { return chunks[cidx(cx, cy, cz)]; }

    void fill_terrain_noise_10_16_grass_stone();
    BlockType get_global(int gx, int gy, int gz) const;
};
