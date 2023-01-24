#pragma once
#include "pch.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace rvr::math {
struct Pose {
    glm::quat orientation_;
    glm::vec3 position_;

    Pose();
    Pose(const XrPosef& xr_pose);
    Pose(const glm::vec3& position, const glm::quat& orientation);
    Pose(const glm::mat3& matrix);
    Pose(const glm::mat4& matrix);

    glm::mat4 ToMat4() const;

    void Translate(const glm::vec3& offset);
    void Translate(float offset_x, float offset_y, float offset_z);
    void Rotate(const glm::vec3& axis, float angle);
    void Rotate(float axis_x, float axis_y, float axis_z, float angle);

    Pose Translated(const glm::vec3& offset) const;
    Pose Translated(float offset_x, float offset_y, float offset_z) const;
    Pose Rotated(const glm::vec3& axis, float angle) const;
    Pose Rotated(float axis_x, float axis_y, float axis_z, float angle) const;

    static Pose Indentity();

}; // Pose
} //namespace rvr::math