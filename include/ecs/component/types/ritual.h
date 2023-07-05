/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <ecs/component/component.h>
#include <ecs/ecs_info.h>
#include <ecs/entity/entity.h>
#include <ecs/component/types/collider.h>

namespace rvr {
class Collider;

class Ritual : public Component {
public:
    Ritual(type::EntityId id);
    Ritual(const Ritual &other) = delete;
    Ritual(const Ritual &other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    virtual ~Ritual() = default;
    virtual void Begin();
    virtual void Update(float delta);
    virtual void OnTriggered(Collider* collider);
    virtual void OnTimeout();
    bool canUpdate;
};
}
