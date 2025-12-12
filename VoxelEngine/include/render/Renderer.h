#pragma once

#include "render/GLShader.h"
#include "render/TextureArray.h"
#include "mesh/VoxelMesher.h"

#include <vector>
#include <glm/glm.hpp>

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool init();
    void upload_mesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds);
    void render(const glm::mat4& mvp);

private:
    ShaderProgram m_prog;
    TextureArray  m_tex;

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    GLsizei m_index_count = 0;
    GLint m_u_mvp = -1;
};
