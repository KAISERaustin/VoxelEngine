#include "render/Renderer.h"

#include <cstddef> // offsetof
#include <iostream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

Renderer::~Renderer()
{
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);

    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;
    m_index_count = 0;
}

bool Renderer::init()
{
    const char* vs_source = R"GLSL(
#version 450 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_norm;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in uint a_layer;

uniform mat4 u_mvp;

out vec3 v_norm;
out vec2 v_uv;
flat out uint v_layer;

void main()
{
    v_norm = a_norm;
    v_uv = a_uv;
    v_layer = a_layer;
    gl_Position = u_mvp * vec4(a_pos, 1.0);
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

    m_u_mvp = m_prog.uniform_location("u_mvp");

    if (!m_tex.create_grass_stone_16()) {
        return false;
    }

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_vbo);
    glCreateBuffers(1, &m_ebo);

    return true;
}

void Renderer::upload_mesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds)
{
    m_index_count = static_cast<GLsizei>(inds.size());

    glNamedBufferData(m_vbo, static_cast<GLsizeiptr>(verts.size() * sizeof(Vertex)), verts.data(), GL_STATIC_DRAW);
    glNamedBufferData(m_ebo, static_cast<GLsizeiptr>(inds.size() * sizeof(uint32_t)), inds.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, static_cast<GLsizei>(sizeof(Vertex)));
    glVertexArrayElementBuffer(m_vao, m_ebo);

    // pos
    glEnableVertexArrayAttrib(m_vao, 0);
    glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(Vertex, pos)));
    glVertexArrayAttribBinding(m_vao, 0, 0);

    // normal
    glEnableVertexArrayAttrib(m_vao, 1);
    glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(Vertex, normal)));
    glVertexArrayAttribBinding(m_vao, 1, 0);

    // uv
    glEnableVertexArrayAttrib(m_vao, 2);
    glVertexArrayAttribFormat(m_vao, 2, 2, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(Vertex, uv)));
    glVertexArrayAttribBinding(m_vao, 2, 0);

    // layer (integer attribute)
    glEnableVertexArrayAttrib(m_vao, 3);
    glVertexArrayAttribIFormat(m_vao, 3, 1, GL_UNSIGNED_INT, static_cast<GLuint>(offsetof(Vertex, layer)));
    glVertexArrayAttribBinding(m_vao, 3, 0);
}

void Renderer::render(const glm::mat4& mvp)
{
    m_prog.use();
    glUniformMatrix4fv(m_u_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

    m_tex.bind_unit(0);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);
}
