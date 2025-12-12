#include "mesh/VoxelMesher.h"

#include <algorithm>

uint32_t tex_layer_for_block(BlockType t)
{
    // texture array layers: 0 = grass, 1 = stone
    switch (t) {
    case BlockType::Grass: return 0;
    case BlockType::Stone: return 1;
    default:               return 1;
    }
}

static void emit_face(std::vector<Vertex>& out_verts,
    std::vector<uint32_t>& out_inds,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c,
    const glm::vec3& d,
    const glm::vec3& n,
    uint32_t layer)
{
    const uint32_t base = static_cast<uint32_t>(out_verts.size());

    const glm::vec2 uv0(0.0f, 0.0f);
    const glm::vec2 uv1(1.0f, 0.0f);
    const glm::vec2 uv2(1.0f, 1.0f);
    const glm::vec2 uv3(0.0f, 1.0f);

    out_verts.push_back(Vertex{ a, n, uv0, layer });
    out_verts.push_back(Vertex{ b, n, uv1, layer });
    out_verts.push_back(Vertex{ c, n, uv2, layer });
    out_verts.push_back(Vertex{ d, n, uv3, layer });

    out_inds.push_back(base + 0);
    out_inds.push_back(base + 1);
    out_inds.push_back(base + 2);

    out_inds.push_back(base + 2);
    out_inds.push_back(base + 3);
    out_inds.push_back(base + 0);
}

void build_world_mesh(const World& world,
    const glm::vec3& world_origin,
    std::vector<Vertex>& out_verts,
    std::vector<uint32_t>& out_inds)
{
    out_verts.clear();
    out_inds.clear();

    for (int gz = 0; gz < WORLD_SIZE_Z; ++gz) {
        for (int gy = 0; gy < WORLD_SIZE_Y; ++gy) {
            for (int gx = 0; gx < WORLD_SIZE_X; ++gx) {
                const BlockType bt = world.get_global(gx, gy, gz);
                if (bt == BlockType::Air) continue;

                const uint32_t layer = tex_layer_for_block(bt);

                const float fx = static_cast<float>(gx) + world_origin.x;
                const float fy = static_cast<float>(gy) + world_origin.y;
                const float fz = static_cast<float>(gz) + world_origin.z;

                const glm::vec3 p000(fx, fy, fz);
                const glm::vec3 p100(fx + 1, fy, fz);
                const glm::vec3 p010(fx, fy + 1, fz);
                const glm::vec3 p110(fx + 1, fy + 1, fz);

                const glm::vec3 p001(fx, fy, fz + 1);
                const glm::vec3 p101(fx + 1, fy, fz + 1);
                const glm::vec3 p011(fx, fy + 1, fz + 1);
                const glm::vec3 p111(fx + 1, fy + 1, fz + 1);

                if (world.get_global(gx + 1, gy, gz) == BlockType::Air) emit_face(out_verts, out_inds, p101, p100, p110, p111, glm::vec3(1, 0, 0), layer);
                if (world.get_global(gx - 1, gy, gz) == BlockType::Air) emit_face(out_verts, out_inds, p000, p001, p011, p010, glm::vec3(-1, 0, 0), layer);
                if (world.get_global(gx, gy + 1, gz) == BlockType::Air) emit_face(out_verts, out_inds, p011, p111, p110, p010, glm::vec3(0, 1, 0), layer);
                if (world.get_global(gx, gy - 1, gz) == BlockType::Air) emit_face(out_verts, out_inds, p000, p100, p101, p001, glm::vec3(0, -1, 0), layer);
                if (world.get_global(gx, gy, gz + 1) == BlockType::Air) emit_face(out_verts, out_inds, p001, p101, p111, p011, glm::vec3(0, 0, 1), layer);
                if (world.get_global(gx, gy, gz - 1) == BlockType::Air) emit_face(out_verts, out_inds, p100, p000, p010, p110, glm::vec3(0, 0, -1), layer);
            }
        }
    }
}
