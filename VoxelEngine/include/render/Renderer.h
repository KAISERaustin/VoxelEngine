#pragma once

#include "render/GLShader.h"
#include "render/TextureArray.h"
#include "mesh/VoxelMesher.h"

#include <glm/glm.hpp>
#include <vector>

struct ChunkMesh
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei index_count = 0;

    bool is_valid() const { return vao != 0 && index_count > 0; }

    void destroy();
};

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool init();

    ChunkMesh create_chunk_mesh(const std::vector<PackedVertex>& verts,
        const std::vector<uint32_t>& inds);

    void begin_frame(const glm::mat4& vp);
    void draw_chunk(const ChunkMesh& mesh, const glm::vec3& chunk_origin);

private:
    ShaderProgram m_prog;
    TextureArray  m_tex;

    GLint m_u_vp = -1;
    GLint m_u_chunk_origin = -1;
};
