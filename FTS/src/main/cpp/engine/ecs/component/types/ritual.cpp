#include <common.h>
#include <ecs/component/types/ritual.h>

namespace rvr {
Ritual::Ritual(type::EntityId pId): Component(ComponentType::Ritual, pId), canUpdate(true){}

void Ritual::Begin() {}

void Ritual::Update(float delta) {}

void Ritual::OnTriggered(Collider *collider) {}

Component *Ritual::Clone(type::EntityId newEntityId) {
    auto ritual = new Ritual(newEntityId);
    ritual->canUpdate = this->canUpdate;
    return ritual;
}
}