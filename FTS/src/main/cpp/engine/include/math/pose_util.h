#pragma once
#include "pch.h"
#include <glm/glm.hpp>

namespace rvr::math::pose {
    XrPosef Identity();
    XrPosef Translation(const XrVector3f& translation);
    XrPosef RotateCCWAboutYAxis(float radians, XrVector3f translation);
} // namespace rvr::math::pose