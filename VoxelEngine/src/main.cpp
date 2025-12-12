#include <iostream>
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "platform/Window.h"
#include "render/Camera.h"
#include "render/CameraController.h"
#include "render/Renderer.h"
#include "world/World.h"
#include "mesh/VoxelMesher.h"

int main()
{
    Window window(1280, 720, "Voxel Engine");
    if (!window.init()) {
        return 1;
    }

    std::cout << "GL_VENDOR   : " << glGetString(GL_VENDOR) << "\n";
    std::cout << "GL_RENDERER : " << glGetString(GL_RENDERER) << "\n";
    std::cout << "GL_VERSION  : " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL        : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Camera camera;
    camera.update_vectors();

    CameraController cam_ctrl(camera);

    Renderer renderer;
    if (!renderer.init()) {
        return 1;
    }

    // World on heap (avoids large stack frame warnings)
    auto world = std::make_unique<World>();
    world->fill_terrain_noise_10_16_grass_stone();

    const glm::vec3 world_origin(
        -static_cast<float>(WORLD_SIZE_X) * 0.5f,
        0.0f,
        -static_cast<float>(WORLD_SIZE_Z) * 0.5f
    );

    std::vector<Vertex> verts;
    std::vector<uint32_t> inds;
    build_world_mesh(*world, world_origin, verts, inds);

    std::cout << "World mesh: " << verts.size() << " verts, " << inds.size() << " indices\n";

    renderer.upload_mesh(verts, inds);

    double last_time = window.time_seconds();

    while (!window.should_close()) {
        const double now = window.time_seconds();
        const float dt = static_cast<float>(now - last_time);
        last_time = now;

        cam_ctrl.update(window, dt);

        const int fb_w = window.framebuffer_width();
        const int fb_h = window.framebuffer_height();
        const float aspect = (fb_h > 0) ? (static_cast<float>(fb_w) / static_cast<float>(fb_h)) : 1.0f;

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::mat4 model = glm::mat4(1.0f);
        const glm::mat4 view = camera.view_matrix();
        const glm::mat4 proj = camera.projection_matrix(aspect, 0.1f, 2000.0f);

        const glm::mat4 mvp = proj * view * model;

        renderer.render(mvp);

        window.swap_buffers();
        window.poll_events();
    }

    window.shutdown();
    return 0;
}
