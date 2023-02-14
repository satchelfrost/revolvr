#include <math/pose.h>
#include <math/matrix_util.h>
#include <math/pose_util.h>
#include <math/qauternion_util.h>
#include <math/vector_util.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rvr::math {

Pose::Pose()
: orientation_(glm::identity<glm::quat>()),
  position_(glm::zero<glm::vec3>()) {}

Pose::Pose(const XrPosef& xr_pose)
: orientation_(quaternion::ToQuat(xr_pose.orientation)),
  position_(vector::ToVec3(xr_pose.position)) {}

Pose::Pose(const glm::vec3& position, const glm::quat& orientation)
: orientation_(orientation), position_(position) {}

Pose::Pose(const glm::mat3& matrix)
: orientation_(glm::toQuat(matrix)),
  position_(glm::zero<glm::vec3>()) {}

Pose::Pose(const glm::mat4& matrix)
: orientation_(glm::toQuat(matrix)),
  position_(matrix::GetPosition(matrix)) {}

glm::mat4 Pose::ToMat4() const {
    glm::mat4 rotation_matrix = glm::toMat4(orientation_);
    glm::mat4 translation_matrix = glm::translate(glm::identity<glm::mat4>(), position_);

    return rotation_matrix * translation_matrix;
}

glm::mat3 Pose::ToMat3() const {
    return glm::toMat3(orientation_);
}


void Pose::Translate(const glm::vec3& offset) {
    position_ += offset;
}

void Pose::Translate(float offset_x, float offset_y, float offset_z) {
    Translate(glm::vec3(offset_x, offset_y, offset_z));
}

void Pose::Rotate(const glm::vec3& axis, float angle) {
    orientation_ = glm::rotate(orientation_, angle, axis);
}

void Pose::Rotate(float axis_x, float axis_y, float axis_z, float angle) {
    Rotate(glm::vec3(axis_x, axis_y, axis_z), angle);
}

Pose Pose::Translated(const glm::vec3& offset) const {
    return {position_ + offset, orientation_};
}

Pose Pose::Translated(float offset_x, float offset_y, float offset_z) const {
    return Translated(glm::vec3(offset_x, offset_y, offset_z));
}

Pose Pose::Rotated(const glm::vec3& axis, float angle) const {
    return {position_, glm::rotate(orientation_, angle, axis)};
}

Pose Pose::Rotated(float axis_x, float axis_y, float axis_z, float angle) const {
    return Rotated(glm::vec3(axis_x, axis_y, axis_z), angle);
}

Pose Pose::Indentity() {
    return {glm::zero<glm::vec3>(), glm::identity<glm::quat>()}; // Same as returning Pose()
}

} // namespace rvr::math