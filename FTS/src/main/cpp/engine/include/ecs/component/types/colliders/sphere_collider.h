#pragma once

#include <ecs/component/types/collider.h>

namespace rvr {
class SphereCollider : public Collider {
public:
    SphereCollider(type::EntityId pId, float pRadius);
    virtual bool TestCollision(Collider* other) override;
    const float radius;
};
}