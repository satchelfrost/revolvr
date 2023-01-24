#include "pch.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "math/matrix_util.h"
#include "math/pose_util.h"
#include "math/qauternion_util.h"
#include "math/vector_util.h"

namespace rvr::math {

namespace matrix {
    glm::vec3 GetXAxis(const glm::mat4& matrix) {
        return glm::column(matrix, 0);
    }

    glm::vec3 GetYAxis(const glm::mat4& matrix) {
        return glm::column(matrix, 1);
    }

    glm::vec3 GetZAxis(const glm::mat4& matrix) {
        return glm::column(matrix, 2);
    }

    glm::vec3 GetXAxis(const glm::mat3& matrix) {
        return glm::column(matrix, 0);
    }

    glm::vec3 GetYAxis(const glm::mat3& matrix) {
        return glm::column(matrix, 1);
    }

    glm::vec3 GetZAxis(const glm::mat3& matrix) {
        return glm::column(matrix, 2);
    }

    glm::vec3 GetScale(const glm::mat4& matrix) {
        glm::vec3 scale;
        scale.x = glm::length(GetXAxis(matrix));
        scale.y = glm::length(GetYAxis(matrix));
        scale.z = glm::length(GetZAxis(matrix));
        return scale;
    }

    glm::vec3 GetScale(const glm::mat3& matrix) {
        glm::vec3 scale;
        scale.x = glm::length(GetXAxis(matrix));
        scale.y = glm::length(GetYAxis(matrix));
        scale.z = glm::length(GetZAxis(matrix));
        return scale;
    }

    glm::vec3 GetPosition(const glm::mat4& matrix) {
        return glm::column(matrix, 3);
    }
} // namespace matrix

namespace pose {

    XrPosef Identity() {
        XrPosef t{};
        t.orientation.w = 1;
        return t;
    }

    XrPosef Translation(const XrVector3f& translation) {
        XrPosef t = Identity();
        t.position = translation;
        return t;
    }

    XrPosef RotateCCWAboutYAxis(float radians, XrVector3f translation) {
        XrPosef t = Identity();
        t.orientation.x = 0.f;
        t.orientation.y = std::sin(radians * 0.5f);
        t.orientation.z = 0.f;
        t.orientation.w = std::cos(radians * 0.5f);
        t.position = translation;
        return t;
    }
} // namespace pose

namespace quaternion {

    glm::quat ToQuat(const XrQuaternionf& quaternion) {
        return {quaternion.w, quaternion.x, quaternion.y, quaternion.z };
    }

    XrQuaternionf ToXrQuaternionf(const glm::quat& quaternion) {
        return { quaternion.x, quaternion.y, quaternion.z, quaternion.w };
    }

    glm::quat FromEuler(glm::vec3 euler_angles) {
        // YXZ Ordering
        // Conversion to quaternion as listed in
        // https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770024290.pdf (page A-6)

        float half_y_angle = euler_angles.y * 0.5f;
        float half_x_angle = euler_angles.x * 0.5f;
        float half_z_angle = euler_angles.z * 0.5f;

        float cos_hy = cos(half_y_angle);
        float sin_hy = sin(half_y_angle);
        float cos_hx = cos(half_x_angle);
        float sin_hx = sin(half_x_angle);
        float cos_hz = cos(half_z_angle);
        float sin_hz = sin(half_z_angle);

        return {
                sin_hy * sin_hx * sin_hz + cos_hy * cos_hx * cos_hz,
                sin_hy * cos_hx * sin_hz + cos_hy * sin_hx * cos_hz,
                sin_hy * cos_hx * cos_hz - cos_hy * sin_hx * sin_hz,
                -sin_hy * sin_hx * cos_hz + cos_hy * cos_hx * sin_hz
        };
    }

    glm::quat FromEuler(float angle_x, float angle_y, float angle_z) {
        return FromEuler(glm::vec3(angle_x, angle_y, angle_z));
    }
} // namespace quaternion

namespace vector {

    XrVector3f Zero() {
        return {0.0f, 0.0f, 0.0f};
    }

    XrVector3f One() {
        return {1.0f, 1.0f, 1.0f};
    }

    glm::vec3 ToVec3(const XrVector3f& vec) {
        return { vec.x, vec.y, vec.z };
    }

    glm::vec4 ToVec4(const XrVector4f& vec) {
        return { vec.x, vec.y, vec.z, vec.w };
    }

    glm::vec4 ToVec4(const XrVector3f& xyz, float w) {
        return { xyz.x, xyz.y, xyz.z, w };
    }

    XrVector3f ToXrVector3f(const glm::vec3& vec) {
        return {vec.x, vec.y, vec.z };
    }

    XrVector4f ToXrVector4f(const glm::vec4& vec) {
        return { vec.x, vec.y, vec.z, vec.w };
    }

    XrVector4f ToXrVector4f(const glm::vec3& xyz, float w) {
        return { xyz.x, xyz.y, xyz.z, w };
    }

} // namespace vector

} // namespace rvr::math
