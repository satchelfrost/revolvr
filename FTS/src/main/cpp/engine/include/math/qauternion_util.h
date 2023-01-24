#pragma once
#include "pch.h"
#include <glm/glm.hpp>

namespace rvr::math::quaternion {
    glm::quat ToQuat(const XrQuaternionf& quaternion);
    XrQuaternionf ToXrQuaternionf(const glm::quat& quaternion);

    glm::quat FromEuler(glm::vec3 euler_angles);
    glm::quat FromEuler(float angle_x, float angle_y, float angle_z);
} // namespace rvr::math::quaternion