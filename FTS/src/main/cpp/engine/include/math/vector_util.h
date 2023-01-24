#pragma once
#include "pch.h"
#include <glm/glm.hpp>

namespace rvr::math::vector {
    XrVector3f Zero();
    XrVector3f One();
    glm::vec3 ToVec3(const XrVector3f& vec);
    glm::vec4 ToVec4(const XrVector4f& vec);
    glm::vec4 ToVec4(const XrVector3f& xyz, float w);
    XrVector3f ToXrVector3f(const glm::vec3& vec);
    XrVector4f ToXrVector4f(const glm::vec4& vec);
    XrVector4f ToXrVector4f(const glm::vec3& xyz, float w);
} // namespace rvr::math::vector