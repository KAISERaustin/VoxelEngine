#include "render/GLShader.h"

#include <iostream>

ShaderProgram::~ShaderProgram()
{
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

GLuint ShaderProgram::compile(GLenum type, const char* src)
{
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);

    GLint ok = GL_FALSE;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint log_len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &log_len);
        std::string log(static_cast<size_t>(log_len), '\0');
        glGetShaderInfoLog(sh, log_len, nullptr, log.data());
        std::cerr << "Shader compile failed:\n" << log << "\n";
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

bool ShaderProgram::build_from_sources(const char* vs_source, const char* fs_source)
{
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    GLuint vs = compile(GL_VERTEX_SHADER, vs_source);
    if (!vs) return false;

    GLuint fs = compile(GL_FRAGMENT_SHADER, fs_source);
    if (!fs) {
        glDeleteShader(vs);
        return false;
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint log_len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_len);
        std::string log(static_cast<size_t>(log_len), '\0');
        glGetProgramInfoLog(prog, log_len, nullptr, log.data());
        std::cerr << "Program link failed:\n" << log << "\n";
        glDeleteProgram(prog);
        return false;
    }

    m_program = prog;
    return true;
}

void ShaderProgram::use() const
{
    glUseProgram(m_program);
}

GLint ShaderProgram::uniform_location(const char* name) const
{
    return glGetUniformLocation(m_program, name);
}
