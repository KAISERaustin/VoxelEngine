#include "render/Renderer.h"

#include <cstddef>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

void ChunkMesh::destroy()
{
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    vao = 0;
    vbo = 0;
    ebo = 0;
    index_count = 0;
}

Renderer::~Renderer()
{
    // Chunk meshes are owned by caller; they must destroy them.
}

bool Renderer::init()
{
    const char* vs_source = R"GLSL(
#version 450 core

layout (location = 0) in uvec3 a_pos;   // uint16
layout (location = 1) in uvec2 a_uv;    // uint16 fixed-point
layout (location = 2) in uint  a_nl;    // packed normal+layer

uniform mat4 u_vp;
uniform vec3 u_chunk_origin;

out vec3 v_norm;
out vec2 v_uv;
flat out uint v_layer;

const float UV_SCALE = 4096.0;

vec3 decode_normal(uint n)
{
    // 0:+X 1:-X 2:+Y 3:-Y 4:+Z 5:-Z
    if (n == 0u) return vec3( 1, 0, 0);
    if (n == 1u) return vec3(-1, 0, 0);
    if (n == 2u) return vec3( 0, 1, 0);
    if (n == 3u) return vec3( 0,-1, 0);
    if (n == 4u) return vec3( 0, 0, 1);
    return            vec3( 0, 0,-1);
}

void main()
{
    uint n = (a_nl & 0xFFu);
    v_layer = (a_nl >> 8) & 0xFFu;

    v_norm = decode_normal(n);
    v_uv = vec2(a_uv) / UV_SCALE;

    vec3 world_pos = u_chunk_origin + vec3(a_pos);
    gl_Position = u_vp * vec4(world_pos, 1.0);
}
)GLSL";

    const char* fs_source = R"GLSL(
#version 450 core

in vec3 v_norm;
in vec2 v_uv;
flat in uint v_layer;

layout (binding = 0) uniform sampler2DArray u_tex;

out vec4 frag_color;

void main()
{
    vec3 n = normalize(v_norm);
    vec3 light_dir = normalize(vec3(0.4, 1.0, 0.2));
    float ndotl = max(dot(n, light_dir), 0.0);

    vec3 albedo = texture(u_tex, vec3(v_uv, float(v_layer))).rgb;
    vec3 color = albedo * (0.25 + 0.75 * ndotl);

    frag_color = vec4(color, 1.0);
}
)GLSL";

    if (!m_prog.build_from_sources(vs_source, fs_source)) {
        return false;
    }

    m_u_vp = m_prog.uniform_location("u_vp");
    m_u_chunk_origin = m_prog.uniform_location("u_chunk_origin");

    if (!m_tex.create_grass_stone_16()) {
        return false;
    }

    return true;
}

ChunkMesh Renderer::create_chunk_mesh(const std::vector<PackedVertex>& verts,
    const std::vector<uint32_t>& inds)
{
    ChunkMesh mesh;
    if (verts.empty() || inds.empty()) {
        return mesh;
    }

    mesh.index_count = (GLsizei)inds.size();

    glCreateVertexArrays(1, &mesh.vao);
    glCreateBuffers(1, &mesh.vbo);
    glCreateBuffers(1, &mesh.ebo);

    glNamedBufferData(mesh.vbo, (GLsizeiptr)(verts.size() * sizeof(PackedVertex)), verts.data(), GL_STATIC_DRAW);
    glNamedBufferData(mesh.ebo, (GLsizeiptr)(inds.size() * sizeof(uint32_t)), inds.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(mesh.vao, 0, mesh.vbo, 0, (GLsizei)sizeof(PackedVertex));
    glVertexArrayElementBuffer(mesh.vao, mesh.ebo);

    // a_pos (uvec3) from uint16 (px,py,pz)
    glEnableVertexArrayAttrib(mesh.vao, 0);
    glVertexArrayAttribIFormat(mesh.vao, 0, 3, GL_UNSIGNED_SHORT, (GLuint)offsetof(PackedVertex, px));
    glVertexArrayAttribBinding(mesh.vao, 0, 0);

    // a_uv (uvec2) from uint16 (u,v)
    glEnableVertexArrayAttrib(mesh.vao, 1);
    glVertexArrayAttribIFormat(mesh.vao, 1, 2, GL_UNSIGNED_SHORT, (GLuint)offsetof(PackedVertex, u));
    glVertexArrayAttribBinding(mesh.vao, 1, 0);

    // a_nl (uint) from uint32 (nl)
    glEnableVertexArrayAttrib(mesh.vao, 2);
    glVertexArrayAttribIFormat(mesh.vao, 2, 1, GL_UNSIGNED_INT, (GLuint)offsetof(PackedVertex, nl));
    glVertexArrayAttribBinding(mesh.vao, 2, 0);

    return mesh;
}

void Renderer::begin_frame(const glm::mat4& vp)
{
    m_prog.use();
    glUniformMatrix4fv(m_u_vp, 1, GL_FALSE, glm::value_ptr(vp));

    m_tex.bind_unit(0);
}

void Renderer::draw_chunk(const ChunkMesh& mesh, const glm::vec3& chunk_origin)
{
    if (!mesh.is_valid()) return;

    glUniform3f(m_u_chunk_origin, chunk_origin.x, chunk_origin.y, chunk_origin.z);

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, nullptr);
}
