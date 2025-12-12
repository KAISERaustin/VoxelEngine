#pragma once

#include "render/Camera.h"
#include "platform/Window.h"

class CameraController
{
public:
    explicit CameraController(Camera& cam);

    void update(Window& window, float dt_seconds);

private:
    Camera& m_cam;
};
