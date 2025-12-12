// VoxelEngine.cpp - Entry point for the voxel engine.

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static GLuint compile_shader(GLenum type, const char* source)
{
	GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

	GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if ( !ok ) {
        GLint log_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        std::string log(static_cast<size_t>(log_len), '\0');
        glGetShaderInfoLog(shader, log_len, nullptr, log.data());
        std::cerr << "Shader compile failed:\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint create_program(const char* vs_source, const char* fs_source)
{
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_source);
    if (!vs) return 0;

    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_source);
    if (!fs)
    {
        glDeleteShader(vs);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint log_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
        std::string log(static_cast<size_t>(log_len), '\0');
        glGetProgramInfoLog(program, log_len, nullptr, log.data());
        std::cerr << "Shader compile failed:\n" << log << "\n";
        glDeleteProgram(program);
        return 0;
    }
    return program;

}

int main()
{
    std::cout << "Running: " << __FILE__ << "\n";

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    // Request an OpenGL 4.5 core context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _WIN32
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Voxel Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        std::cout << "GL_VENDOR   : " << glGetString(GL_VENDOR) << "\n";
        std::cout << "GL_RENDERER : " << glGetString(GL_RENDERER) << "\n";
        std::cout << "GL_VERSION  : " << glGetString(GL_VERSION) << "\n";
        std::cout << "GLSL        : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    int fb_w = 0, fb_h = 0;
    glfwGetFramebufferSize(window, &fb_w, &fb_h);
    glViewport(0, 0, fb_w, fb_h);

    const char* vs_source = R"GLSL(
#version 450 core
layout (location = 0) in vec2 a_pos;

void main()
{
    gl_Position = vec4(a_pos, 0.0, 1.0);
}
)GLSL";

    const char* fs_source = R"GLSL(
#version 450 core
out vec4 frag_color;

void main()
{
    frag_color = vec4(1.0, 0.6, 0.2, 1.0);
}
)GLSL";

    GLuint program = create_program(vs_source, fs_source);
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const float vertices[] = {
        0.0f, 0.5f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    GLuint vao = 0, vbo = 0;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);

    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float) * 2);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
