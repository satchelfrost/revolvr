#include <ecs/system/ritual_system.h>
#include <ecs/component/component_type.h>
#include <ecs/ecs.h>

namespace rvr::system::ritual{
void Update(float dt) {
    for (auto entityId : ECS::Instance()->GetEids(ComponentType::Ritual)) {
        auto ritual = ECS::Instance()->GetComponent<Ritual>(entityId);
        if (ritual->canUpdate)
            ritual->Update(dt);
    }
}
void Begin() {
    for (auto entityId : ECS::Instance()->GetEids(ComponentType::Ritual)) {
        auto ritual = ECS::Instance()->GetComponent<Ritual>(entityId);
        ritual->Begin();
    }
}
}