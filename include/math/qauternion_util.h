/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once
#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace rvr::math::quaternion {
    // Returns a glm::quat made from a XrQuaternionf
    glm::quat ToQuat(const XrQuaternionf& quaternion);

    // Returns a XrQuaternionf made from a glm::quat
    XrQuaternionf ToXrQuaternionf(const glm::quat& quaternion);

    // Creates a quaternion representing an orientation using the given Euler Angles (in degrees) using the YXZ Ordering
    glm::quat FromEuler(glm::vec3 euler_angles);

    // Creates a quaternion representing an orientation using the given Euler Angles (in degrees) using the YXZ Ordering
    glm::quat FromEuler(float angle_x, float angle_y, float angle_z);

    // Returns quaternion identity {0,0,0,1}
    glm::quat Identity();

    // Creates a quaternion representing an orientation given the axis angle of rotation (in degrees)
    glm::quat FromAxisAngle(glm::vec3 axis, float angle);
}
