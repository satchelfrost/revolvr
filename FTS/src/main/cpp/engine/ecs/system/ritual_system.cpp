#include <ecs/system/ritual_system.h>
#include <ecs/component/component_type.h>
#include <ecs/ecs.h>

namespace rvr {
void RitualSystem::Update(float dt) {
    for (auto entityId : ECS::Instance()->GetEids(ComponentType::Ritual)) {
        auto ritual = ECS::Instance()->GetComponent<Ritual>(entityId);
        if (ritual->HasImpl() && ritual->canUpdate)
            ritual->Update(dt);
    }
}
void RitualSystem::Begin() {
    for (auto entityId : ECS::Instance()->GetEids(ComponentType::Ritual)) {
        auto ritual = ECS::Instance()->GetComponent<Ritual>(entityId);
        if (ritual->HasImpl())
            ritual->Begin();
    }
}
}