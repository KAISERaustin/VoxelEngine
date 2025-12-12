#include "render/CameraController.h"

#include <GLFW/glfw3.h>

CameraController::CameraController(Camera& cam)
    : m_cam(cam)
{
}

void CameraController::update(Window& window, float dt_seconds)
{
    // Mouse look
    const auto [dx, dy] = window.consume_mouse_delta();
    if (dx != 0.0f || dy != 0.0f) {
        m_cam.process_mouse(dx, dy);
    }

    // Keyboard move
    float speed = m_cam.move_speed;
    if (window.key_down(GLFW_KEY_LEFT_SHIFT)) {
        speed *= 2.5f;
    }

    const float v = speed * dt_seconds;

    if (window.key_down(GLFW_KEY_W)) m_cam.pos += m_cam.front * v;
    if (window.key_down(GLFW_KEY_S)) m_cam.pos -= m_cam.front * v;

    if (window.key_down(GLFW_KEY_A)) m_cam.pos -= m_cam.right * v;
    if (window.key_down(GLFW_KEY_D)) m_cam.pos += m_cam.right * v;

    if (window.key_down(GLFW_KEY_SPACE))        m_cam.pos += m_cam.world_up * v;
    if (window.key_down(GLFW_KEY_LEFT_CONTROL)) m_cam.pos -= m_cam.world_up * v;

    if (window.key_down(GLFW_KEY_ESCAPE)) {
        window.set_should_close(true);
    }
}
