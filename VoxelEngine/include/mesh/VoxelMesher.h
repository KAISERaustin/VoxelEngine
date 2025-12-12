#pragma once

#include "world/World.h"

#include <cstdint>
#include <vector>

struct PackedVertex
{
    // bits:
    //   0..7   = normal index (0:+X, 1:-X, 2:+Y, 3:-Y, 4:+Z, 5:-Z)
    //   8..15  = texture layer (0=grass, 1=stone)
    uint32_t nl = 0;

    // position in chunk-local voxel coordinates (0..16)
    uint16_t px = 0;
    uint16_t py = 0;
    uint16_t pz = 0;

    // UV in fixed-point units (see UV_UNIT in mesher and UV_SCALE in shader)
    uint16_t u = 0;
    uint16_t v = 0;
};

static_assert(sizeof(PackedVertex) == 16, "PackedVertex should be 16 bytes");

void build_chunk_mesh_greedy(const World& world,
    int cx, int cy, int cz,
    std::vector<PackedVertex>& out_verts,
    std::vector<uint32_t>& out_inds);
