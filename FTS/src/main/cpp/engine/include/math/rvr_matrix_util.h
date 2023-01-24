#pragma once
#include "pch.h"
#include <glm/glm.hpp>

namespace rvr::math::matrix {
    glm::vec3 GetXAxis(const glm::mat4& matrix);
    glm::vec3 GetYAxis(const glm::mat4& matrix);
    glm::vec3 GetZAxis(const glm::mat4& matrix);
    glm::vec3 GetXAxis(const glm::mat3& matrix);
    glm::vec3 GetYAxis(const glm::mat3& matrix);
    glm::vec3 GetZAxis(const glm::mat3& matrix);

    glm::vec3 GetScale(const glm::mat4& matrix);
    glm::vec3 GetScale(const glm::mat3& matrix);

    glm::vec3 GetPosition(const glm::mat4& matrix);
}// namespace rvr::math::matrix