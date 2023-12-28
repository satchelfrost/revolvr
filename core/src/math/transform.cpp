/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "math/transform.h"
#include "glm/ext.hpp"
#include "glm/gtx/quaternion.hpp"

namespace rvr::math {
Transform::Transform()
: pose_(Pose::Identity()), scale_(1, 1, 1) {}

Transform::Transform(const Pose& pose)
: pose_(pose), scale_(1, 1, 1) {}

Transform::Transform(const Pose& pose, const glm::vec3& scale)
: pose_(pose), scale_(scale) {}

Transform::Transform(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
: pose_(position, orientation), scale_(scale) {}

Transform::Transform(const glm::mat4& matrix)
: pose_(Pose(matrix)), scale_(matrix::GetScale(matrix)) {}

glm::mat4 Transform::ToMat4() const {
    glm::mat4 pose_matrix = pose_.ToMat4();
    glm::mat4 scale_matrix = glm::scale(glm::identity<glm::mat4>(), scale_);
    return pose_matrix * scale_matrix;
}

glm::vec3 Transform::GetXAxis() const {
    return matrix::GetXAxis(pose_.ToMat4());
}

glm::vec3 Transform::GetYAxis() const {
    return matrix::GetYAxis(pose_.ToMat4());
}

glm::vec3 Transform::GetZAxis() const {
    return matrix::GetZAxis(pose_.ToMat4());
}

Pose Transform::GetPose() const {
    return pose_;
}

void Transform::SetPose(const glm::vec3& position, const glm::quat& orientation) {
    pose_ = {position, orientation};
}

void Transform::SetPose(const Pose& pose) {
    this->pose_ = pose;
}

glm::vec3 Transform::GetScale() const {
    return scale_;
}

void Transform::SetScale(float x, float y, float z) {
    scale_ = {x, y, z};
}

void Transform::SetScale(const glm::vec3& scale) {
    this->scale_ = scale;
}

glm::quat Transform::GetOrientation() const {
    return pose_.GetOrientation();
}

void Transform::SetOrientation(const glm::mat3& orientation) {
    pose_.SetOrientation(orientation);
}

void Transform::SetOrientation(const glm::quat& orientation) {
    pose_.SetOrientation(orientation);
}

glm::vec3 Transform::GetPosition() const {
    return pose_.GetPosition();
}

void Transform::SetPosition(float x, float y, float z) {
    pose_.SetPosition(x, y, z);
}

void Transform::SetPosition(const glm::vec3& position) {
    pose_.SetPosition(position);
}

void Transform::Translate(const glm::vec3& offset) {
    pose_.Translate(offset);
}

void Transform::Rotate(const glm::vec3& axis, float theta) {
    pose_.Rotate(axis, theta);
}

void Transform::Scale(const glm::vec3& scale) {
    this->scale_ *= scale;
}

Transform Transform::Translated(glm::vec3 offset) const {
    return {pose_.Translated(offset), scale_};
}

Transform Transform::Rotated(glm::vec3 axis, float theta) const {
    return {pose_.Rotated(axis, theta), scale_};
}

Transform Transform::Scaled(glm::vec3 scale) const {
    return {pose_, this->scale_ * scale};
}

Transform Transform::Identity() {
    return {Pose::Identity(), glm::vec3(1, 1, 1)};
}

bool Transform::operator==(const Transform &other) const {
    return pose_ == other.pose_ && scale_ == other.scale_;
}
}
