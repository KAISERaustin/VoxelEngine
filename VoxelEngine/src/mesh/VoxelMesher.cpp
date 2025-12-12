#include "mesh/VoxelMesher.h"

#include <array>
#include <algorithm>

static inline bool is_solid(BlockType t)
{
    return t != BlockType::Air;
}

static inline uint8_t layer_for_block(BlockType t)
{
    // TextureArray layers: 0 = grass, 1 = stone
    switch (t) {
    case BlockType::Grass: return 0;
    case BlockType::Stone: return 1;
    default:               return 1;
    }
}

static inline uint8_t normal_index_for_axis_sign(int axis, int sign)
{
    // 0:+X 1:-X 2:+Y 3:-Y 4:+Z 5:-Z
    if (axis == 0) return (sign > 0) ? 0 : 1;
    if (axis == 1) return (sign > 0) ? 2 : 3;
    return (sign > 0) ? 4 : 5;
}

static inline uint32_t pack_nl(uint8_t normal_idx, uint8_t layer)
{
    return (uint32_t)normal_idx | ((uint32_t)layer << 8);
}

// Fixed-point UV units. With max quad size 16, 16 * 4096 = 65536, so clamp to 65535.
static constexpr uint16_t UV_UNIT = 4096;

static inline uint16_t uv_encode(int blocks)
{
    const int v = blocks * (int)UV_UNIT;
    return (uint16_t)std::clamp(v, 0, 65535);
}

static void emit_quad(std::vector<PackedVertex>& out_verts,
    std::vector<uint32_t>& out_inds,
    const std::array<int, 3>& p0,
    const std::array<int, 3>& p1,
    const std::array<int, 3>& p2,
    const std::array<int, 3>& p3,
    uint8_t normal_idx,
    uint8_t layer,
    int u_len,
    int v_len,
    bool flip_uv)
{
    const uint32_t base = (uint32_t)out_verts.size();

    const uint16_t u0 = 0;
    const uint16_t v0 = 0;
    const uint16_t u1 = uv_encode(u_len);
    const uint16_t v1 = uv_encode(v_len);

    // Basic tiling UVs; optionally flip U to keep some faces from mirroring hard.
    auto make_vert = [&](const std::array<int, 3>& p, uint16_t uu, uint16_t vv) -> PackedVertex {
        PackedVertex pv;
        pv.nl = pack_nl(normal_idx, layer);
        pv.px = (uint16_t)p[0];
        pv.py = (uint16_t)p[1];
        pv.pz = (uint16_t)p[2];
        pv.u = uu;
        pv.v = vv;
        return pv;
        };

    if (!flip_uv) {
        out_verts.push_back(make_vert(p0, u0, v0));
        out_verts.push_back(make_vert(p1, u1, v0));
        out_verts.push_back(make_vert(p2, u1, v1));
        out_verts.push_back(make_vert(p3, u0, v1));
    }
    else {
        out_verts.push_back(make_vert(p0, u1, v0));
        out_verts.push_back(make_vert(p1, u0, v0));
        out_verts.push_back(make_vert(p2, u0, v1));
        out_verts.push_back(make_vert(p3, u1, v1));
    }

    out_inds.push_back(base + 0);
    out_inds.push_back(base + 1);
    out_inds.push_back(base + 2);

    out_inds.push_back(base + 2);
    out_inds.push_back(base + 3);
    out_inds.push_back(base + 0);
}

void build_chunk_mesh_greedy(const World& world,
    int cx, int cy, int cz,
    std::vector<PackedVertex>& out_verts,
    std::vector<uint32_t>& out_inds)
{
    out_verts.clear();
    out_inds.clear();

    const int dims[3] = { CHUNK_X, CHUNK_Y, CHUNK_Z };

    const int base_gx = cx * CHUNK_X;
    const int base_gy = cy * CHUNK_Y;
    const int base_gz = cz * CHUNK_Z;

    struct MaskCell
    {
        bool filled = false;
        BlockType t = BlockType::Air;
        uint8_t normal_idx = 0;
        uint8_t layer = 0;
    };

    // For each principal axis, build a mask per slice and greedy-merge rectangles.
    for (int d = 0; d < 3; ++d) {
        const int u = (d + 1) % 3;
        const int v = (d + 2) % 3;

        const int du = dims[u];
        const int dv = dims[v];

        std::vector<MaskCell> mask((size_t)(du * dv));

        // slice runs from -1 to dims[d]-1, faces lie on plane slice+1
        for (int slice = -1; slice < dims[d]; ++slice) {

            // Build mask for this slice
            for (int j = 0; j < dv; ++j) {
                for (int i = 0; i < du; ++i) {
                    MaskCell cell;

                    int c0[3] = { 0, 0, 0 };
                    c0[u] = i;
                    c0[v] = j;
                    c0[d] = slice;

                    int c1[3] = { c0[0], c0[1], c0[2] };
                    c1[d] = slice + 1;

                    const BlockType a = (slice >= 0)
                        ? world.get_global(base_gx + c0[0], base_gy + c0[1], base_gz + c0[2])
                        : BlockType::Air;

                    const BlockType b = (slice + 1 < dims[d])
                        ? world.get_global(base_gx + c1[0], base_gy + c1[1], base_gz + c1[2])
                        : world.get_global(base_gx + c1[0], base_gy + c1[1], base_gz + c1[2]);

                    // Face exists if exactly one side is solid
                    if (is_solid(a) && !is_solid(b)) {
                        cell.filled = true;
                        cell.t = a;
                        cell.layer = layer_for_block(a);
                        cell.normal_idx = normal_index_for_axis_sign(d, +1);
                    }
                    else if (!is_solid(a) && is_solid(b)) {
                        cell.filled = true;
                        cell.t = b;
                        cell.layer = layer_for_block(b);
                        cell.normal_idx = normal_index_for_axis_sign(d, -1);
                    }

                    mask[(size_t)(i + j * du)] = cell;
                }
            }

            // Greedy merge rectangles in mask
            for (int j = 0; j < dv; ++j) {
                for (int i = 0; i < du; ) {
                    const MaskCell& c = mask[(size_t)(i + j * du)];
                    if (!c.filled) {
                        ++i;
                        continue;
                    }

                    // Compute width
                    int w = 1;
                    while (i + w < du) {
                        const MaskCell& c2 = mask[(size_t)((i + w) + j * du)];
                        if (!c2.filled) break;
                        if (c2.t != c.t || c2.normal_idx != c.normal_idx || c2.layer != c.layer) break;
                        ++w;
                    }

                    // Compute height
                    int h = 1;
                    bool done = false;
                    while (j + h < dv && !done) {
                        for (int k = 0; k < w; ++k) {
                            const MaskCell& c2 = mask[(size_t)((i + k) + (j + h) * du)];
                            if (!c2.filled || c2.t != c.t || c2.normal_idx != c.normal_idx || c2.layer != c.layer) {
                                done = true;
                                break;
                            }
                        }
                        if (!done) ++h;
                    }

                    // Emit quad
                    // Quad lies on plane at coord[d] = slice+1, spanning u:[i..i+w], v:[j..j+h]
                    const int plane = slice + 1;

                    std::array<int, 3> x = { 0, 0, 0 };
                    x[d] = plane;
                    x[u] = i;
                    x[v] = j;

                    std::array<int, 3> duv = { 0, 0, 0 };
                    duv[u] = w;

                    std::array<int, 3> dvv = { 0, 0, 0 };
                    dvv[v] = h;

                    // Vertex order depends on which side the face points to
                    const bool positive = (c.normal_idx == normal_index_for_axis_sign(d, +1));

                    std::array<int, 3> p0, p1, p2, p3;
                    if (positive) {
                        p0 = x;
                        p1 = { x[0] + duv[0], x[1] + duv[1], x[2] + duv[2] };
                        p2 = { x[0] + duv[0] + dvv[0], x[1] + duv[1] + dvv[1], x[2] + duv[2] + dvv[2] };
                        p3 = { x[0] + dvv[0], x[1] + dvv[1], x[2] + dvv[2] };
                    }
                    else {
                        // flipped winding for negative-facing quads
                        p0 = x;
                        p1 = { x[0] + dvv[0], x[1] + dvv[1], x[2] + dvv[2] };
                        p2 = { x[0] + duv[0] + dvv[0], x[1] + duv[1] + dvv[1], x[2] + duv[2] + dvv[2] };
                        p3 = { x[0] + duv[0], x[1] + duv[1], x[2] + duv[2] };
                    }

                    const bool flip_uv = (d == 0); // minor consistency tweak; not required
                    emit_quad(out_verts, out_inds, p0, p1, p2, p3, c.normal_idx, c.layer, w, h, flip_uv);

                    // Clear mask cells covered by this rectangle
                    for (int y = 0; y < h; ++y) {
                        for (int xw = 0; xw < w; ++xw) {
                            mask[(size_t)((i + xw) + (j + y) * du)] = MaskCell{};
                        }
                    }

                    i += w;
                }
            }
        }
    }
}
