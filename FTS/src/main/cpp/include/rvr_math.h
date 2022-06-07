#pragma once

namespace RVRMath {
    namespace Pose {
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
    }  // namespace Pose
}  // namespace Math
