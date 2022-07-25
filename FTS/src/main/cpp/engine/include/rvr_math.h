#pragma once
#include "pch.h"

namespace RVRMath {
namespace Pose {
XrPosef Identity();
XrPosef Translation(const XrVector3f& translation);
XrPosef RotateCCWAboutYAxis(float radians, XrVector3f translation);
} // namespace Pose

namespace Vector {
XrVector3f Zero();
XrVector3f One();
} // namespace Vector
} // namespace Math
