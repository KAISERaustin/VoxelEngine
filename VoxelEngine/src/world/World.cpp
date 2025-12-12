#include "world/World.h"
#include "world/Noise.h"

void World::fill_terrain_noise_10_16_grass_stone()
{
    for (int gz = 0; gz < WORLD_SIZE_Z; ++gz) {
        for (int gx = 0; gx < WORLD_SIZE_X; ++gx) {
            const int h = terrain_height_10_16(gx, gz);

            for (int gy = 0; gy < WORLD_SIZE_Y; ++gy) {
                BlockType t = BlockType::Air;
                if (gy < h) {
                    t = (gy == h - 1) ? BlockType::Grass : BlockType::Stone;
                }

                const int cx = gx / CHUNK_X;
                const int cy = gy / CHUNK_Y;
                const int cz = gz / CHUNK_Z;

                const int lx = gx % CHUNK_X;
                const int ly = gy % CHUNK_Y;
                const int lz = gz % CHUNK_Z;

                chunk_at(cx, cy, cz).set_local(lx, ly, lz, t);
            }
        }
    }
}

BlockType World::get_global(int gx, int gy, int gz) const
{
    if (gx < 0 || gx >= WORLD_SIZE_X ||
        gy < 0 || gy >= WORLD_SIZE_Y ||
        gz < 0 || gz >= WORLD_SIZE_Z) {
        return BlockType::Air;
    }

    const int cx = gx / CHUNK_X;
    const int cy = gy / CHUNK_Y;
    const int cz = gz / CHUNK_Z;

    const int lx = gx % CHUNK_X;
    const int ly = gy % CHUNK_Y;
    const int lz = gz % CHUNK_Z;

    return chunk_at(cx, cy, cz).get_local(lx, ly, lz);
}
