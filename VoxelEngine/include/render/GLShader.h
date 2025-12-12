#pragma once

#include <string>
#include <glad/glad.h>

class ShaderProgram
{
public:
    ShaderProgram() = default;
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    bool build_from_sources(const char* vs_source, const char* fs_source);

    void use() const;
    GLuint id() const { return m_program; }

    GLint uniform_location(const char* name) const;

private:
    static GLuint compile(GLenum type, const char* src);

private:
    GLuint m_program = 0;
};
