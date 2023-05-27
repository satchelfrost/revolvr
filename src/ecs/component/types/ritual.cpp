#include <common.h>
#include <ecs/component/types/ritual.h>

namespace rvr {
Ritual::Ritual(type::EntityId pId) : Component(ComponentType::Ritual, pId), canUpdate(true){}

void Ritual::Begin() {}

void Ritual::Update(float delta) {}

void Ritual::OnTriggered(Collider *collider) {}

void Ritual::OnTimeout() {}

Component *Ritual::Clone(type::EntityId newEntityId) {
    return new Ritual(*this, newEntityId);
}

Ritual::Ritual(const Ritual &other, type::EntityId newEntityId) :
Component(ComponentType::Ritual, newEntityId), canUpdate(other.canUpdate){}
}