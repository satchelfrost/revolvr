#include <ecs/component/types/colliders/sphere_collider.h>
#include <common.h>

namespace rvr {
SphereCollider::SphereCollider(type::EntityId pId, float pRadius) :
 Collider(Collider::ColliderType::Sphere, pId), radius(pRadius) {}

bool SphereCollider::TestSphere(Collider* collider) {
    if (collider->type != Collider::ColliderType::Sphere)
        THROW(Fmt("TestSphere() error, expecting ColliderType::Sphere, received %s",
                   Collider::ColliderTypeToString(collider->type).c_str()))
    auto sphere = reinterpret_cast<SphereCollider*>(collider);

    float radiusSum = radius + sphere->radius;
    return true;
}

bool SphereCollider::TestAABB(Collider* collider) { return false; }

bool SphereCollider::TestOBB(Collider* collider) { return false; }

}
