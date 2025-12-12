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
#include "render/Frustum.h"
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

    // World on heap (keeps stack small)
    auto world = std::make_unique<World>();
    world->fill_terrain_noise_10_16_grass_stone();

    // Center the world around origin in X/Z
    const glm::vec3 world_origin(
        -static_cast<float>(WORLD_SIZE_X) * 0.5f,
        0.0f,
        -static_cast<float>(WORLD_SIZE_Z) * 0.5f
    );

    // Build per-chunk meshes (greedy meshing + packed vertices)
    std::vector<PackedVertex> verts;
    std::vector<uint32_t> inds;

    std::vector<ChunkMesh> chunk_meshes;
    std::vector<glm::vec3> chunk_origins;

    chunk_meshes.reserve(WORLD_CHUNKS_X * WORLD_CHUNKS_Y * WORLD_CHUNKS_Z);
    chunk_origins.reserve(WORLD_CHUNKS_X * WORLD_CHUNKS_Y * WORLD_CHUNKS_Z);

    for (int cz = 0; cz < WORLD_CHUNKS_Z; ++cz) {
        for (int cy = 0; cy < WORLD_CHUNKS_Y; ++cy) {
            for (int cx = 0; cx < WORLD_CHUNKS_X; ++cx) {

                build_chunk_mesh_greedy(*world, cx, cy, cz, verts, inds);

                ChunkMesh mesh = renderer.create_chunk_mesh(verts, inds);

                const glm::vec3 chunk_origin =
                    world_origin +
                    glm::vec3((float)(cx * CHUNK_X), (float)(cy * CHUNK_Y), (float)(cz * CHUNK_Z));

                chunk_meshes.push_back(mesh);
                chunk_origins.push_back(chunk_origin);
            }
        }
    }

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

        const glm::mat4 view = camera.view_matrix();
        const glm::mat4 proj = camera.projection_matrix(aspect, 0.1f, 2000.0f);
        const glm::mat4 vp = proj * view;

        // Frustum for this frame
        const Frustum fr = Frustum::from_vp(vp);

        renderer.begin_frame(vp);

        const glm::vec3 chunk_size((float)CHUNK_X, (float)CHUNK_Y, (float)CHUNK_Z);

        for (size_t i = 0; i < chunk_meshes.size(); ++i) {
            const ChunkMesh& m = chunk_meshes[i];
            if (!m.is_valid()) continue;

            const glm::vec3 bmin = chunk_origins[i];
            const glm::vec3 bmax = chunk_origins[i] + chunk_size;

            if (!fr.intersects_aabb(bmin, bmax)) {
                continue;
            }

            renderer.draw_chunk(m, chunk_origins[i]);
        }

        window.swap_buffers();
        window.poll_events();
    }

    for (auto& m : chunk_meshes) {
        m.destroy();
    }

    window.shutdown();
    return 0;
}
