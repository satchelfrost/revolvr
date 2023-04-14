#include "ecs/component/types/mesh.h"

namespace rvr {
Mesh::Mesh(type::EntityId pId) : Component(ComponentType::Mesh, pId), visible(true) {}

Component *Mesh::Clone(type::EntityId newEntityId) {
    auto mesh = new Mesh(newEntityId);
    mesh->visible = this->visible;
    return mesh;
}
}