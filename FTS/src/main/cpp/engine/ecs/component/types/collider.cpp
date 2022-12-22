#include <ecs/component/types/collider.h>
#include <common.h>

namespace rvr {
Collider::Collider(ColliderType pType, type::EntityId pId) :
 Component(ComponentType::Collider, pId), type(pType) {}

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
}
