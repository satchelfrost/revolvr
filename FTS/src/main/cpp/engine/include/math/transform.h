#pragma once

#include "math.h"

namespace rvr::math {
class Transform {
private:
    Pose pose_;
    glm::vec3 scale_;

public:
    Transform();
    Transform(const Pose& pose);
    Transform(const Pose& pose, const glm::vec3& scale);
    Transform(const glm::mat4& matrix);

    glm::mat4 ToMat4() const;

    glm::vec3 GetXAxis() const;
    glm::vec3 GetYAxis() const;
    glm::vec3 GetZAxis() const;

    glm::vec3 GetScale() const;
    void SetScale(const glm::vec3& scale);
    void SetScale(float x, float y, float z);

    glm::quat GetOrientation() const;
    void SetOrientation(const glm::mat3& orientation);
    void SetOrientation(const glm::quat& orientation);

    glm::vec3 GetPosition() const;
    void SetPosition(float x, float y, float z);
    void SetPosition(const glm::vec3& position);

    // No return transformation is done in-place
    void Translate(const glm::vec3& offset);
    void Rotate(const glm::vec3& axis, float theta);
    void Scale(const glm::vec3& scale);

    // Return the transform with the given transformation
    Transform Translated(glm::vec3 offset) const;
    Transform Rotated(glm::vec3 axis, float theta) const;
    Transform Scaled(glm::vec3 scale) const;

    static Transform Identity();
};
} // namespace rvr::math
