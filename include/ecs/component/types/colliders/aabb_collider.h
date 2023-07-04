/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <ecs/component/types/collider.h>

namespace rvr {
class AABBCollider : public Collider {
public:
    AABBCollider(const AABBCollider& other) = delete;
    AABBCollider(const AABBCollider& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    AABBCollider(type::EntityId pId, float halfX, float halfY, float halfZ);
    virtual bool TestCollision(Collider* other) override;
    const float halfX;
    const float halfY;
    const float halfZ;
};
}
