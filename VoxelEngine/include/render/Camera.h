#pragma once

#include <glm/glm.hpp>

struct Camera
{
    glm::vec3 pos = glm::vec3(0.0f, 30.0f, 140.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;
    float pitch = -15.0f;

    float move_speed = 12.0f;
    float mouse_sens = 0.12f;
    float fov_degrees = 70.0f;

    void update_vectors();

    glm::mat4 view_matrix() const;
    glm::mat4 projection_matrix(float aspect, float z_near = 0.1f, float z_far = 2000.0f) const;

    void process_mouse(float xoffset, float yoffset);
};
