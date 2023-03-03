#include <ecs/component/types/colliders/aabb_collider.h>
#include <common.h>
#include <ecs/ecs.h>
#include <ecs/component/types/spatial.h>

namespace rvr {
AABBCollider::AABBCollider(type::EntityId pId, float halfX, float halfY, float halfZ) :
        Collider(ColliderType::AABB, pId), halfX(halfX), halfY(halfY), halfZ(halfZ) {}

bool AABBCollider::TestCollision(Collider* other) {
    switch (other->type) {
        case ColliderType::AABB: {
            // Get the spatials
            auto otherSpatial = ECS::Instance()->GetComponent<Spatial>(other->id);
            auto otherPos = otherSpatial->GetWorld().GetPosition();
            auto thisSpatial = ECS::Instance()->GetComponent<Spatial>(id);
            auto thisPos = thisSpatial->GetWorld().GetPosition();
            auto otherAABB = reinterpret_cast<AABBCollider*>(other);

            if (abs(thisPos.x - otherPos.x) > (halfX + otherAABB->halfX))
                return false;
            if (abs(thisPos.y - otherPos.y) > (halfY + otherAABB->halfY))
                return false;
            if (abs(thisPos.z - otherPos.z) > (halfZ + otherAABB->halfZ))
                return false;

            return true;
        }
        case ColliderType::Sphere:
        case ColliderType::OBB:
            return false;
        default:
            THROW(Fmt("Collider type %d unrecognized", other->type))
    }
}
}
