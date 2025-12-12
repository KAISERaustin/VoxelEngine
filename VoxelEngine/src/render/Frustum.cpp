#include "render/Frustum.h"

#include <cmath>      // std::sqrt
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::transpose

static inline glm::vec4 normalize_plane(const glm::vec4& p)
{
    const float len = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    if (len <= 1e-8f) {
        return p;
    }
    return p / len;
}

Frustum Frustum::from_vp(const glm::mat4& vp)
{
    Frustum f;

    const glm::mat4 t = glm::transpose(vp);

    const glm::vec4 row0 = t[0];
    const glm::vec4 row1 = t[1];
    const glm::vec4 row2 = t[2];
    const glm::vec4 row3 = t[3];

    f.planes[0] = normalize_plane(row3 + row0); // left
    f.planes[1] = normalize_plane(row3 - row0); // right
    f.planes[2] = normalize_plane(row3 + row1); // bottom
    f.planes[3] = normalize_plane(row3 - row1); // top
    f.planes[4] = normalize_plane(row3 + row2); // near
    f.planes[5] = normalize_plane(row3 - row2); // far

    return f;
}

bool Frustum::intersects_aabb(const glm::vec3& bmin, const glm::vec3& bmax) const
{
    for (const glm::vec4& p : planes) {
        const glm::vec3 n(p.x, p.y, p.z);

        const glm::vec3 v(
            (n.x >= 0.0f) ? bmax.x : bmin.x,
            (n.y >= 0.0f) ? bmax.y : bmin.y,
            (n.z >= 0.0f) ? bmax.z : bmin.z
        );

        if (glm::dot(n, v) + p.w < 0.0f) {
            return false;
        }
    }
    return true;
}
