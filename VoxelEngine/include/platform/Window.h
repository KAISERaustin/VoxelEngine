#pragma once

#include <string>
#include <utility>

struct GLFWwindow;

class Window
{
public:
    Window(int width, int height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool init();                // glfwInit + create window + make context + gladLoadGLLoader
    void shutdown();            // safe to call multiple times

    void poll_events();
    void swap_buffers();

    bool should_close() const;
    void set_should_close(bool v);

    GLFWwindow* native_handle() const;

    int framebuffer_width() const;
    int framebuffer_height() const;

    // Returns mouse delta since last call: (dx, dy)
    std::pair<float, float> consume_mouse_delta();

    // Input helpers
    bool key_down(int glfw_key) const;

    // Time
    double time_seconds() const;

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

private:
    int m_init_w = 0;
    int m_init_h = 0;
    std::string m_title;

    GLFWwindow* m_window = nullptr;

    int m_fb_w = 0;
    int m_fb_h = 0;

    bool m_first_mouse = true;
    float m_last_x = 0.0f;
    float m_last_y = 0.0f;

    float m_mouse_dx = 0.0f;
    float m_mouse_dy = 0.0f;
};
