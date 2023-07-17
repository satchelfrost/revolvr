/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/component/types/collider.h>
#include <common.h>
#include <global_context.h>

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

void Collider::Collided(Collider *other) {
    auto entity = GlobalContext::Inst()->GetECS()->GetEntity(id);
    if (entity->HasComponent(ComponentType::Ritual)) {
        auto ritual = GlobalContext::Inst()->GetECS()->GetComponent<Ritual>(id);
        ritual->OnTriggered(other);
    }
}
}
