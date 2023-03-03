#include <ecs/component/types/colliders/sphere_collider.h>
#include <common.h>
#include <ecs/ecs.h>
#include <ecs/component/types/spatial.h>

namespace rvr {
SphereCollider::SphereCollider(type::EntityId pId, float pRadius) :
 Collider(ColliderType::Sphere, pId), radius(pRadius) {}

bool SphereCollider::TestCollision(Collider* other) {
    switch (other->type) {
        case ColliderType::Sphere: {
            // Get the spatials
            auto otherSpatial = ECS::Instance()->GetComponent<Spatial>(other->id);
            auto thisSpatial = ECS::Instance()->GetComponent<Spatial>(id);

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
            THROW(Fmt("Collider type %d unrecognized", other->type))
    }
}
}
