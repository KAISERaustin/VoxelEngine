#pragma once

#include "world/World.h"

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    uint32_t  layer;
};

uint32_t tex_layer_for_block(BlockType t);

void build_world_mesh(const World& world,
    const glm::vec3& world_origin,
    std::vector<Vertex>& out_verts,
    std::vector<uint32_t>& out_inds);
