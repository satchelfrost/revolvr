#include "ecs/component/types/mesh.h"
#include <global_context.h>

namespace rvr {
Mesh::Mesh(type::EntityId pId) : Component(ComponentType::Mesh, pId), visible(true) {}

Component *Mesh::Clone(type::EntityId newEntityId) {
    auto mesh = new Mesh(newEntityId);
    mesh->visible = this->visible;
    return mesh;
}

void Mesh::SetVisibilityRecursive(bool visibility) {
    auto entity = GlobalContext::Inst()->GetECS()->GetEntity(id);
    for (auto child : entity->GetChildren()) {
        auto mesh = GlobalContext::Inst()->GetECS()->GetComponent<Mesh>(child->id);
        CHECK_MSG(mesh, Fmt("Entity with id %d has no mesh", child->id));
        mesh->SetVisibilityRecursive(visibility);
    }
    visible = visibility;
}
}