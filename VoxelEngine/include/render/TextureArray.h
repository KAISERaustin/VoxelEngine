#pragma once

#include <glad/glad.h>

class TextureArray
{
public:
    TextureArray() = default;
    ~TextureArray();

    TextureArray(const TextureArray&) = delete;
    TextureArray& operator=(const TextureArray&) = delete;

    bool create_grass_stone_16();      // 2-layer 16x16 RGBA8 array
    void bind_unit(GLuint unit) const;

    GLuint id() const { return m_tex; }

private:
    GLuint m_tex = 0;
};
