#pragma once

#include "platform/Window.h"

class Input
{
public:
    explicit Input(Window& w) : m_window(w) {}

    bool down(int glfw_key) const { return m_window.key_down(glfw_key); }

private:
    Window& m_window;
};
