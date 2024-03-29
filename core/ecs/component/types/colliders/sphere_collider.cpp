/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/component/types/colliders/sphere_collider.h>
#include <common.h>
#include <global_context.h>
#include <ecs/component/types/spatial.h>

namespace rvr {
SphereCollider::SphereCollider(type::EntityId pId, float pRadius) :
Collider(ColliderType::Sphere, pId), radius(pRadius) {}

bool SphereCollider::TestCollision(Collider* other) {
    switch (other->type) {
        case ColliderType::Sphere: {
            // Get the spatials
            auto otherSpatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(other->id);
            auto thisSpatial =  GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(id);

            glm::vec3 dir = thisSpatial->GetWorld().GetPosition() - otherSpatial->GetWorld().GetPosition();

            float dist2 = glm::dot(dir, dir);

            auto otherSphere = reinterpret_cast<SphereCollider*>(other);
            float radiusSum = radius + otherSphere->radius;
            if (dist2 <= (radiusSum * radiusSum))
                return true;
        }
        case ColliderType::AABB:
        case ColliderType::OBB:
            return false;
        default:
            THROW(Fmt("Collider type %d unrecognized", other->type));
    }
}

Component *SphereCollider::Clone(type::EntityId newEntityId) {
    return new SphereCollider(*this, newEntityId);
}

SphereCollider::SphereCollider(const SphereCollider &other, type::EntityId newEntityId) :
Collider(ColliderType::Sphere, newEntityId), radius(other.radius) {}
}