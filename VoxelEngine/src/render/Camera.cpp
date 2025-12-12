#include "render/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::update_vectors()
{
    glm::vec3 f;
    f.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    f.y = std::sin(glm::radians(pitch));
    f.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front = glm::normalize(f);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::view_matrix() const
{
    return glm::lookAt(pos, pos + front, up);
}

glm::mat4 Camera::projection_matrix(float aspect, float z_near, float z_far) const
{
    return glm::perspective(glm::radians(fov_degrees), aspect, z_near, z_far);
}

void Camera::process_mouse(float xoffset, float yoffset)
{
    xoffset *= mouse_sens;
    yoffset *= mouse_sens;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    update_vectors();
}
