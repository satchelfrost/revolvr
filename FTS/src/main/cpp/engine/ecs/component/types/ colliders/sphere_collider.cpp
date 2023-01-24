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

            // TODO: Might be replaced with "Vector3 d = thisSpatial->transform.position - otherSpatial.transform.position"
            XrVector3f dir = {thisSpatial->worldPose.position.x - otherSpatial->worldPose.position.x,
                              thisSpatial->worldPose.position.y - otherSpatial->worldPose.position.y,
                              thisSpatial->worldPose.position.z - otherSpatial->worldPose.position.z};

            // TODO: Might be replaced with "dist2 = Dot(d, d)"
            float dist2 = (dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);

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
