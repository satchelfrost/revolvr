#pragma once

#include <ecs/component/types/collider.h>

namespace rvr {
class SphereCollider : public Collider {
public:
    SphereCollider(type::EntityId pId, float pRadius);
    virtual bool TestSphere(Collider* sphere) override;
    virtual bool TestAABB(Collider* aabb) override;
    virtual bool TestOBB(Collider* obb) override;
    const float radius;
};
}
