/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include <ecs/component/types/collider.h>

namespace rvr {
class SphereCollider : public Collider {
public:
    SphereCollider(const SphereCollider& other) = delete;
    SphereCollider(const SphereCollider& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    SphereCollider(type::EntityId pId, float pRadius);
    virtual bool TestCollision(Collider* other) override;
    const float radius;
};
}
