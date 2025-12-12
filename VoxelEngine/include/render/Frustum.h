#pragma once

#include <glm/glm.hpp>

struct Frustum
{
    // Plane equation: ax + by + cz + d = 0
    glm::vec4 planes[6]{};

    static Frustum from_vp(const glm::mat4& vp);

    bool intersects_aabb(const glm::vec3& bmin, const glm::vec3& bmax) const;
};
