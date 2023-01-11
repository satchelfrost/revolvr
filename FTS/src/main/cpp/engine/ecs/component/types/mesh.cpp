#include "ecs/component/types/mesh.h"

namespace rvr {
Mesh::Mesh(type::EntityId pId) : Component(ComponentType::Mesh, pId), visible(true) {}
}