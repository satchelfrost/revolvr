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
#include <glm/ext.hpp>

namespace rvr::math {
class Pose {
private:
    glm::quat orientation_;
    glm::vec3 position_;

public:
    Pose();
    Pose(const XrPosef& xr_pose);
    Pose(const glm::vec3& position, const glm::quat& orientation);
    Pose(const glm::mat3& matrix);
    Pose(const glm::mat4& matrix);
    bool operator==(const Pose& other) const;

    // Converts the pose into a 4x4 Matrix with a scale of {1,1,1}
    glm::mat4 ToMat4() const;

    // Converts the pose into a 3x3 Matrix with a scale of {1,1,1}
    glm::mat3 ToMat3() const;

    // Converts the pose into a XrPosef
    XrPosef ToXrPosef() const;

    glm::vec3 GetPosition() const;
    void SetPosition(float x, float y, float z);
    void SetPosition(const glm::vec3& position);

    glm::quat GetOrientation() const;
    void SetOrientation(const glm::mat3& orientation);
    void SetOrientation(const glm::quat& orientation);

    // Translates the Pose position by the given offset
    void Translate(const glm::vec3& offset);

    // Translates the Pose position by the given offset_x, offset_y and offset_z
    void Translate(float offset_x, float offset_y, float offset_z);

    // Rotates the Pose orientation using a rotation defined by the given axis and angle
    void Rotate(const glm::vec3& axis, float angle);

    // Rotates the Pose orientation using a rotation defined by the given axis_x, axis_y, axis_z and angle
    void Rotate(float axis_x, float axis_y, float axis_z, float angle);

    // Creates a new pose based on the current one translated by the given offset
    Pose Translated(const glm::vec3& offset) const;

    // Creates a new pose based on the current one translated by the given offset_x, offset_y, and offset_z
    Pose Translated(float offset_x, float offset_y, float offset_z) const;

    // Creates a new pose based on the current one rotated by using the given axis and angle
    Pose Rotated(const glm::vec3& axis, float angle) const;

    // Creates a new pose based on the current one rotated by using the given axis_x, axis_y, axis_z and angle
    Pose Rotated(float axis_x, float axis_y, float axis_z, float angle) const;

    // Returns the identity pose defined as position{0,0,0} and orientation{0,0,0,1}
    static Pose Identity();
};
}
