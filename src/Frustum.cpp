#include "Frustum.h"
#include <cmath>

void Frustum::ExtractFromMatrix(const glm::mat4& mat) {
    // GLM is column-major, so mat[col][row]. We extract the 6 planes using Gribb/Hartmann math.

    // Left Plane
    planes[0].normal.x = mat[0][3] + mat[0][0];
    planes[0].normal.y = mat[1][3] + mat[1][0];
    planes[0].normal.z = mat[2][3] + mat[2][0];
    planes[0].distance = mat[3][3] + mat[3][0];

    // Right Plane
    planes[1].normal.x = mat[0][3] - mat[0][0];
    planes[1].normal.y = mat[1][3] - mat[1][0];
    planes[1].normal.z = mat[2][3] - mat[2][0];
    planes[1].distance = mat[3][3] - mat[3][0];

    // Bottom Plane
    planes[2].normal.x = mat[0][3] + mat[0][1];
    planes[2].normal.y = mat[1][3] + mat[1][1];
    planes[2].normal.z = mat[2][3] + mat[2][1];
    planes[2].distance = mat[3][3] + mat[3][1];

    // Top Plane
    planes[3].normal.x = mat[0][3] - mat[0][1];
    planes[3].normal.y = mat[1][3] - mat[1][1];
    planes[3].normal.z = mat[2][3] - mat[2][1];
    planes[3].distance = mat[3][3] - mat[3][1];

    // Near Plane
    planes[4].normal.x = mat[0][3] + mat[0][2];
    planes[4].normal.y = mat[1][3] + mat[1][2];
    planes[4].normal.z = mat[2][3] + mat[2][2];
    planes[4].distance = mat[3][3] + mat[3][2];

    // Far Plane
    planes[5].normal.x = mat[0][3] - mat[0][2];
    planes[5].normal.y = mat[1][3] - mat[1][2];
    planes[5].normal.z = mat[2][3] - mat[2][2];
    planes[5].distance = mat[3][3] - mat[3][2];

    // Normalize all planes so the distance calculations are accurate
    for (int i = 0; i < 6; ++i) {
        planes[i].Normalize();
    }
}

bool Frustum::IsBoxVisible(const glm::vec3& minBounds, const glm::vec3& maxBounds) const {
    // Calculate the center and extents (half-sizes) of the bounding box
    glm::vec3 extents = (maxBounds - minBounds) * 0.5f;
    glm::vec3 center = minBounds + extents;

    for (int i = 0; i < 6; ++i) {
        const Plane& p = planes[i];

        // Calculate the "radius" of the box projected onto the plane's normal
        float r = extents.x * std::abs(p.normal.x) + extents.y * std::abs(p.normal.y)
            + extents.z * std::abs(p.normal.z);

        // Calculate the distance from the box's center to the plane
        float d = glm::dot(p.normal, center) + p.distance;

        // If the center is further behind the plane than the radius, it is completely outside!
        if (d < -r) {
            return false;
        }
    }
    return true; // The box intersects the frustum and should be rendered
}