#include "platform/Window.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Window::Window(int width, int height, const char* title)
    : m_init_w(width), m_init_h(height), m_title(title ? title : "Window")
{
}

Window::~Window()
{
    shutdown();
}

bool Window::init()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _WIN32
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_init_w, m_init_h, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    // Route callbacks to this instance
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, &Window::framebuffer_size_callback);
    glfwSetCursorPosCallback(m_window, &Window::mouse_callback);

    // Lock cursor for FPS-style camera
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
        return false;
    }

    glfwGetFramebufferSize(m_window, &m_fb_w, &m_fb_h);
    glViewport(0, 0, m_fb_w, m_fb_h);

    return true;
}

void Window::shutdown()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Window::poll_events()
{
    glfwPollEvents();
}

void Window::swap_buffers()
{
    glfwSwapBuffers(m_window);
}

bool Window::should_close() const
{
    return glfwWindowShouldClose(m_window) != 0;
}

void Window::set_should_close(bool v)
{
    glfwSetWindowShouldClose(m_window, v ? GLFW_TRUE : GLFW_FALSE);
}

GLFWwindow* Window::native_handle() const
{
    return m_window;
}

int Window::framebuffer_width() const { return m_fb_w; }
int Window::framebuffer_height() const { return m_fb_h; }

std::pair<float, float> Window::consume_mouse_delta()
{
    const float dx = m_mouse_dx;
    const float dy = m_mouse_dy;
    m_mouse_dx = 0.0f;
    m_mouse_dy = 0.0f;
    return { dx, dy };
}

bool Window::key_down(int glfw_key) const
{
    return glfwGetKey(m_window, glfw_key) == GLFW_PRESS;
}

double Window::time_seconds() const
{
    return glfwGetTime();
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    self->m_fb_w = width;
    self->m_fb_h = height;

    glViewport(0, 0, width, height);
}

void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    const float x = static_cast<float>(xpos);
    const float y = static_cast<float>(ypos);

    if (self->m_first_mouse) {
        self->m_last_x = x;
        self->m_last_y = y;
        self->m_first_mouse = false;
        return;
    }

    const float dx = x - self->m_last_x;
    const float dy = self->m_last_y - y;

    self->m_last_x = x;
    self->m_last_y = y;

    self->m_mouse_dx += dx;
    self->m_mouse_dy += dy;
}
