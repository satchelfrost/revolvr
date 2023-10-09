/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/component/types/point_light.h>

namespace rvr {
PointLight::PointLight(type::EntityId pId) : Component(ComponentType::PointLight, pId), intensity_(0.15f),
color_(1.0f, 1.0f, 1.0f) {}

PointLight::PointLight(type::EntityId pId, glm::vec3 color, float intensity) :
Component(ComponentType::PointLight, pId),intensity_(intensity), color_(color) {}

PointLight::PointLight(const PointLight& other, type::EntityId newEntityId) :
Component(ComponentType::PointLight, newEntityId), intensity_(other.intensity_), color_(other.color_){}

Component *PointLight::Clone(type::EntityId newEntityId) {
    return new PointLight(*this, newEntityId);
}

float PointLight::GetIntensity() {
    return intensity_;
}

glm::vec3 PointLight::GetColor() {
    return color_;
}
}
