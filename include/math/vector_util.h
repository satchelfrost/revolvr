#pragma once
#include "pch.h"
#include <glm/glm.hpp>

namespace rvr::math::vector {
    // Returns an XrVector3f set to {0,0,0}
    XrVector3f Zero();

    // Returns an XrVector3f set to {1,1,1}
    XrVector3f One();

    // Converts a XrVector3f to glm::vec3
    glm::vec3 ToVec3(const XrVector3f& vec);

    // Converts a XrVector4f to glm::vec4
    glm::vec4 ToVec4(const XrVector4f& vec);

    // Converts a XrVector3f and a float into a glm::vec4
    glm::vec4 ToVec4(const XrVector3f& xyz, float w);

    // Converts a glm::vec3 to XrVector3f
    XrVector3f ToXrVector3f(const glm::vec3& vec);

    // Converts a glm::vec4 to XrVector4f
    XrVector4f ToXrVector4f(const glm::vec4& vec);

    // Converts a glm::vec3 and a float into a XrVector4f
    XrVector4f ToXrVector4f(const glm::vec3& xyz, float w);
}
