#include <ecs/component/types/collider.h>
#include <common.h>
#include <ecs/system/notification/collision_event.h>

namespace rvr {
Collider::Collider(ColliderType pType, type::EntityId pId) :
 Component(ComponentType::Collider, pId), type(pType) {
    // default event
    collisionEvent_ = new CollisionEvent(this, this);
}

Collider::~Collider() {
    delete collisionEvent_;
}

std::string Collider::ColliderTypeToString(Collider::ColliderType type) {
    switch (type) {
        case ColliderType::Sphere:
            return "Sphere";
        case ColliderType::AABB:
            return "AABB";
        case ColliderType::OBB:
            return "OBB";
        default:
            THROW(Fmt("Collider type %d not found", type))
    }
}

Collider::ColliderType Collider::StrToColliderTypeEnum(const std::string& str) {
    for (int i = 0; i < colliders; i++)
        if (ColliderTypeToString((ColliderType)i) == str)
            return (ColliderType)i;
    THROW(Fmt("Collider Type %s unrecognized", str.c_str()))
}

void Collider::AddObserver(Ritual *ritual) {
    subject_.AddObserver(ritual);
}

void Collider::RemoveObserver(Ritual *ritual) {
    subject_.RemoveObserver(ritual);
}

void Collider::Notify(Collider *other) {
    collisionEvent_->other = other;
    collisionEvent_->src = this;
    subject_.Notify(collisionEvent_);
}
}
