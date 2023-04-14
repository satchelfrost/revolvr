#pragma once

#include <ecs/component/types/collider.h>

namespace rvr {
class AABBCollider : public Collider {
public:
    AABBCollider(type::EntityId pId, float halfX, float halfY, float halfZ);
    virtual Component* Clone(type::EntityId newEntityId) override;
    virtual bool TestCollision(Collider* other) override;
    const float halfX;
    const float halfY;
    const float halfZ;
};
}
