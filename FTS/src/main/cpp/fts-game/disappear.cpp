#include "disappear.h"
#include <ecs/ecs.h>

Disappear::Disappear(rvr::type::EntityId id) : Ritual(id) {
}

void Disappear::Begin() {}
void Disappear::Update(float delta) {}

void Disappear::OnOverlap(rvr::Collider *collider) {
}
