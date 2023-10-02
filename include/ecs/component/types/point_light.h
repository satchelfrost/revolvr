/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <ecs/component/component.h>
#include "ecs/entity/entity.h"

namespace rvr {
class PointLight : public Component {
public:
    PointLight(type::EntityId pId);
    PointLight(const PointLight& other) = delete;
    PointLight(const PointLight& other, type::EntityId pId);
    virtual Component* Clone(type::EntityId newEntityId) override;
};
}
