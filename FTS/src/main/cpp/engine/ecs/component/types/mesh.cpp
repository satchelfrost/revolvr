#include "ecs/component/types/mesh.h"

namespace rvr {
Mesh::Mesh() : Component(ComponentType::Mesh), visible(true) {}
}