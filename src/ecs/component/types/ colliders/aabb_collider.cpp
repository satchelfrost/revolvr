/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/component/types/colliders/aabb_collider.h>
#include <common.h>
#include <ecs/component/types/spatial.h>
#include <global_context.h>

namespace rvr {
AABBCollider::AABBCollider(type::EntityId pId, float halfX, float halfY, float halfZ) :
        Collider(ColliderType::AABB, pId), halfX(halfX), halfY(halfY), halfZ(halfZ) {}

bool AABBCollider::TestCollision(Collider* other) {
    switch (other->type) {
        case ColliderType::AABB: {
            // Get the spatials
            auto otherSpatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(other->id);
            auto otherPos = otherSpatial->GetWorld().GetPosition();
            auto thisSpatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(id);
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

Component *AABBCollider::Clone(type::EntityId newEntityId) {
    return new AABBCollider(*this, newEntityId);
}

AABBCollider::AABBCollider(const AABBCollider &other, type::EntityId newEntityId) :
Collider(ColliderType::Sphere, newEntityId), halfX(other.halfX), halfY(other.halfY), halfZ(other.halfZ) {}
}
