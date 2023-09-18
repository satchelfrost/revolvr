/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "ecs/component/types/mesh.h"
#include <global_context.h>

namespace rvr {
Mesh::Mesh(type::EntityId pId, bool visible) : Component(ComponentType::Mesh, pId), visible_(visible) {}

Mesh::Mesh(const Mesh &other, type::EntityId newEntityId) :
Component(ComponentType::Mesh, newEntityId), visible_(other.visible_),
resourceName_(other.resourceName_) {}

Component *Mesh::Clone(type::EntityId newEntityId) {
    return new Mesh(*this, newEntityId);
}

void Mesh::SetVisibilityRecursive(bool visibility) {
    auto entity = GlobalContext::Inst()->GetECS()->GetEntity(id);
    for (auto child : entity->GetChildren()) {
        auto mesh = GlobalContext::Inst()->GetECS()->GetComponent<Mesh>(child->id);
        CHECK_MSG(mesh, Fmt("Entity with id %d has no mesh", child->id));
        mesh->SetVisibilityRecursive(visibility);
    }
    visible_ = visibility;
}

bool Mesh::IsVisible() const {
    return visible_;
}

bool Mesh::HasResource() const {
    return !resourceName_.empty();
}

std::string Mesh::ResourceName() {
    return resourceName_;
}

void Mesh::SetName(std::string name) {
    resourceName_ = name;
}
}