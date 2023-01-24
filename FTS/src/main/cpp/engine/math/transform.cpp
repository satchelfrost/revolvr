#include "math/transform.h"

#include "glm/ext.hpp"
#include "glm/gtx/quaternion.hpp"

namespace rvr::math {

Transform::Transform()
: pose_(), scale_(1,1,1) {}

Transform::Transform(const Pose& pose, const glm::vec3& scale)
: pose_(pose), scale_(scale) {}

Transform::Transform(const glm::mat4& matrix)
: pose_(Pose(matrix)), scale_(matrix::GetScale(matrix)) {}

glm::mat4 Transform::ToMat4() const {
    glm::mat4 pose_matrix = pose_.ToMat4();
    glm::mat4 scale_matix = glm::scale(glm::identity<glm::mat4>(), scale_);
    return pose_matrix * scale_matix;
}

glm::vec3 Transform::GetXAxis() const {
    return matrix::GetXAxis(GetOrientation());
}

glm::vec3 Transform::GetYAxis() const {
    return matrix::GetYAxis(GetOrientation());
}

glm::vec3 Transform::GetZAxis() const {
    return matrix::GetZAxis(GetOrientation());
}

glm::vec3 Transform::GetScale() const {
    return scale_;
}

void Transform::SetScale(const glm::vec3& scale) {
    scale_ = scale;
}

void Transform::SetScale(float x, float y, float z) {
    SetScale(glm::vec3(x, y, z));
}

glm::mat3 Transform::GetOrientation() const {
    return glm::toMat3(pose_.orientation_);
}

void Transform::SetOrientation(const glm::mat3& orientation) {
    pose_.orientation_ = orientation;
}

glm::vec3 Transform::GetPosition() const {
    return pose_.position_;
}

void Transform::SetPosition(float x, float y, float z) {
    SetPosition(glm::vec3(x, y, z));
}

void Transform::SetPosition(const glm::vec3& position) {
    pose_.position_ = position;
}

void Transform::Translate(const glm::vec3& offset) {
    pose_.Translate(offset);
}

void Transform::Rotate(const glm::vec3& axis, float theta) {
    pose_.Rotate(axis, theta);
}

void Transform::Scale(const glm::vec3& scale) {
    scale_ *= scale;
}

Transform Transform::Translated(glm::vec3 offset) const {
    return {pose_.Translated(offset), scale_};
}

Transform Transform::Rotated(glm::vec3 axis, float theta) const {
    return {pose_.Rotated(axis, theta), scale_};
}

Transform Transform::Scaled(glm::vec3 scale) const {
    return {pose_, scale_ * scale};
}

Transform Transform::Identity() {
    return {};
}

} // namespace rvr::math
