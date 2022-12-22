#include <common.h>
#include "ecs/component/types/ritual.h"

namespace rvr {
Ritual::Ritual(type::EntityId pId): Component(ComponentType::Ritual, pId), canUpdate(true){}

void Ritual::OnOverlap(Collider *collider) {}
}