#pragma once

#include <array>
#include <cstdint>

static constexpr int CHUNK_X = 16;
static constexpr int CHUNK_Y = 16;
static constexpr int CHUNK_Z = 16;

enum class BlockType : uint8_t
{
    Air = 0,
    Grass = 1,
    Stone = 2
};

struct Chunk
{
    std::array<uint8_t, CHUNK_X* CHUNK_Y* CHUNK_Z> blocks{};

    static constexpr int idx(int x, int y, int z)
    {
        return x + CHUNK_X * (y + CHUNK_Y * z);
    }

    BlockType get_local(int x, int y, int z) const
    {
        return static_cast<BlockType>(blocks[idx(x, y, z)]);
    }

    void set_local(int x, int y, int z, BlockType t)
    {
        blocks[idx(x, y, z)] = static_cast<uint8_t>(t);
    }
};
