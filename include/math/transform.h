#pragma once

#include "linear_math.h"

namespace rvr::math {
class Transform {
private:
    Pose pose_;
    glm::vec3 scale_;

public:
    Transform();
    Transform(const Pose& pose);
    Transform(const Pose& pose, const glm::vec3& scale);
    Transform(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);
    Transform(const glm::mat4& matrix);
    bool operator==(const Transform& other) const;

    // Converts the transform into a 4x4 Matrix
    glm::mat4 ToMat4() const;

    // Gets the vector corresponding to the x-axis from a 4x4 Matrix
    glm::vec3 GetXAxis() const;

    // Gets the vector corresponding to the y-axis from a 4x4 Matrix
    glm::vec3 GetYAxis() const;

    // Gets the vector corresponding to the z-axis from a 4x4 Matrix
    glm::vec3 GetZAxis() const;

    Pose GetPose() const;
    void SetPose(const glm::vec3& position, const glm::quat& orientation);
    void SetPose(const Pose& pose);

    glm::vec3 GetScale() const;
    void SetScale(float x, float y, float z);
    void SetScale(const glm::vec3& scale);

    glm::quat GetOrientation() const;
    void SetOrientation(const glm::mat3& orientation);
    void SetOrientation(const glm::quat& orientation);

    glm::vec3 GetPosition() const;
    void SetPosition(float x, float y, float z);
    void SetPosition(const glm::vec3& position);

    // Translates the Transform position by the given offset
    void Translate(const glm::vec3& offset);

    // Rotates the Transform orientation using a rotation defined by the given axis and angle
    void Rotate(const glm::vec3& axis, float theta);

    // Scales the Transform by the given scale
    void Scale(const glm::vec3& scale);

    // Creates a new Transform based on the current one translated by the given offset
    Transform Translated(glm::vec3 offset) const;

    // Creates a new Transform based on the current one rotated by using the given axis and angle
    Transform Rotated(glm::vec3 axis, float theta) const;

    // Creates a new Transform based on the current one scaled by using the given scale
    Transform Scaled(glm::vec3 scale) const;

    // Returns the identity transform defined as position{0,0,0}, orientation{0,0,0,1} and scale{1,1,1}
    static Transform Identity();
};
}
