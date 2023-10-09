/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <ecs/component/component.h>
#include "ecs/entity/entity.h"
#include "glm/vec3.hpp"

namespace rvr {
class PointLight : public Component {
    float intensity_;
    glm::vec3 color_;

public:
    PointLight(type::EntityId pId);
    PointLight(type::EntityId pId, glm::vec3 color, float intensity);
    PointLight(const PointLight& other) = delete;
    PointLight(const PointLight& other, type::EntityId pId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    float GetIntensity();
    glm::vec3 GetColor();
};
}
