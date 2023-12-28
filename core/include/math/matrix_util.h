/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once
#include "pch.h"
#include <glm/glm.hpp>

namespace rvr::math::matrix {
    // Gets the vector corresponding to the x-axis from a 4x4 Matrix
    glm::vec3 GetXAxis(const glm::mat4& matrix);

    // Gets the vector corresponding to the y-axis from a 4x4 Matrix
    glm::vec3 GetYAxis(const glm::mat4& matrix);

    // Gets the vector corresponding to the z-axis from a 4x4 Matrix
    glm::vec3 GetZAxis(const glm::mat4& matrix);

    // Gets the vector corresponding to the x-axis from a 3x3 Matrix
    glm::vec3 GetXAxis(const glm::mat3& matrix);

    // Gets the vector corresponding to the y-axis from a 3x3 Matrix
    glm::vec3 GetYAxis(const glm::mat3& matrix);

    // Gets the vector corresponding to the z-axis from a 3x3 Matrix
    glm::vec3 GetZAxis(const glm::mat3& matrix);

    // Gets the vector corresponding to the scale from a 4x4 Matrix
    glm::vec3 GetScale(const glm::mat4& matrix);

    // Gets the vector corresponding to the scale from a 3x3 Matrix
    glm::vec3 GetScale(const glm::mat3& matrix);

    // Gets the vector corresponding to the position from a 4x4 Matrix
    glm::vec3 GetPosition(const glm::mat4& matrix);

    glm::mat4 CreateProjectionFromXrFOV(const XrFovf fov, const float nearZ, const float farZ);
}
