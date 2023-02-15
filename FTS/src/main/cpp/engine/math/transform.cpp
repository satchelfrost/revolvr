#include "math/transform.h"
#include "glm/ext.hpp"
#include "glm/gtx/quaternion.hpp"

namespace rvr::math {

Transform::Transform()
: pose(Pose::Identity()), scale(1, 1, 1) {}

Transform::Transform(const Pose& pose)
: pose(pose), scale(1, 1, 1) {}

Transform::Transform(const Pose& pose, const glm::vec3& scale)
: pose(pose), scale(scale) {}

Transform::Transform(const glm::mat4& matrix)
: pose(Pose(matrix)), scale(matrix::GetScale(matrix)) {}

glm::mat4 Transform::ToMat4() const {
    glm::mat4 pose_matrix = pose.ToMat4();
    glm::mat4 scale_matix = glm::scale(glm::identity<glm::mat4>(), scale);
    return pose_matrix * scale_matix;
}

glm::vec3 Transform::GetXAxis() const {
    return matrix::GetXAxis(pose.ToMat4());
}

glm::vec3 Transform::GetYAxis() const {
    return matrix::GetYAxis(pose.ToMat4());
}

glm::vec3 Transform::GetZAxis() const {
    return matrix::GetZAxis(pose.ToMat4());
}

void Transform::SetScale(float x, float y, float z) {
    scale = {x, y, z};
}

glm::quat Transform::GetOrientation() const {
    return pose.orientation;
}

void Transform::SetOrientation(const glm::mat3& orientation) {
    pose.orientation = orientation;
}

void Transform::SetOrientation(const glm::quat& orientation) {
    pose.orientation = orientation;
}

glm::vec3 Transform::GetPosition() const {
    return pose.position;
}

void Transform::SetPosition(float x, float y, float z) {
    pose.position = {x, y, z};
}

void Transform::SetPosition(const glm::vec3& position) {
    pose.position = position;
}

void Transform::Translate(const glm::vec3& offset) {
    pose.Translate(offset);
}

void Transform::Rotate(const glm::vec3& axis, float theta) {
    pose.Rotate(axis, theta);
}

void Transform::Scale(const glm::vec3& scale) {
    this->scale *= scale;
}

Transform Transform::Translated(glm::vec3 offset) const {
    return {pose.Translated(offset), scale};
}

Transform Transform::Rotated(glm::vec3 axis, float theta) const {
    return {pose.Rotated(axis, theta), scale};
}

Transform Transform::Scaled(glm::vec3 scale) const {
    return {pose, this->scale * scale};
}

Transform Transform::Identity() {
    return {Pose::Identity(), glm::vec3(1, 1, 1)};
}

} // namespace rvr::math
