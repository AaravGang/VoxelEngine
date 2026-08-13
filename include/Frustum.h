#pragma once
#include <array>
#include <glm/glm.hpp>

// A geometric plane defined by a normal vector and a distance from the origin
struct Plane {
    glm::vec3 normal;
    float distance;

    void Normalize() {
        float mag = glm::length(normal);
        normal /= mag;
        distance /= mag;
    }
};

class Frustum {
private:
    std::array<Plane, 6> planes;

public:
    // Extract the 6 planes of vision directly from the Camera's View-Projection matrix
    void ExtractFromMatrix(const glm::mat4& viewProjMatrix);

    // Fast Axis-Aligned Bounding Box (AABB) intersection test
    bool IsBoxVisible(const glm::vec3& minBounds, const glm::vec3& maxBounds) const;
};