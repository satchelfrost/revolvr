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
            auto otherSphere = reinterpret_cast<SphereCollider*>(other);
            auto otherSpatial = ECS::Instance()->GetComponent<Spatial>(other->id);
            auto thisSpatial = ECS::Instance()->GetComponent<Spatial>(id);

            Entity* thisChild = ECS::Instance()->GetEntity(id);
            auto thisParent = thisChild->GetParent();
            Log::Write(Log::Level::Info, Fmt("This entity %s", thisChild->GetName().c_str()));
            Log::Write(Log::Level::Info, Fmt("This parent id %d", thisParent->id));
            Entity* otherChild = ECS::Instance()->GetEntity(other->id);
            auto otherParent = thisChild->GetParent();
            Log::Write(Log::Level::Info, Fmt("Other entity %s", otherChild->GetName().c_str()));
            Log::Write(Log::Level::Info, Fmt("Other parent id %d", otherParent->id));



            // Might be replaced with "Vector3 d = thisSpatial->transform.position - otherSpatial.transform.position"
            XrVector3f dir = {thisSpatial->worldPose.position.x - otherSpatial->worldPose.position.x,
                              thisSpatial->worldPose.position.y - otherSpatial->worldPose.position.y,
                              thisSpatial->worldPose.position.z - otherSpatial->worldPose.position.z};


            // Might be replaced with "dist2 = Dot(d, d)"
            float dist2 = (dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);

            float radiusSum = radius + otherSphere->radius;
            if (dist2 <= (radiusSum * radiusSum)) {
                Log::Write(Log::Level::Info, Fmt("dist2 %f, rad^2 %.2f", dist2, radiusSum * radiusSum));
                Log::Write(Log::Level::Info, Fmt("this {%.2f, %.2f, %.2f}",
                                                             thisSpatial->worldPose.position.x,
                                                             thisSpatial->worldPose.position.y,
                                                             thisSpatial->worldPose.position.z));
                Log::Write(Log::Level::Info, Fmt("other {%.2f, %.2f, %.2f}",
                                                             otherSpatial->worldPose.position.x,
                                                             otherSpatial->worldPose.position.y,
                                                             otherSpatial->worldPose.position.z));
                return true;
            }
        }
        case ColliderType::AABB:
        case ColliderType::OBB:
            return false;
        default:
            THROW(Fmt("Collider type %d unrecognized", other->type))
    }
    return false;
}
}
