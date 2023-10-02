/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/component/types/point_light.h>

namespace rvr {
PointLight::PointLight(type::EntityId pId) : Component(ComponentType::PointLight, pId) {}

PointLight::PointLight(const PointLight& other, type::EntityId newEntityId) :
Component(ComponentType::PointLight, newEntityId) {}

Component *PointLight::Clone(type::EntityId newEntityId) {
    return new PointLight(*this, newEntityId);
}
}
