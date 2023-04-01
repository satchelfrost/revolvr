#include <ecs/system/ritual_system.h>
#include <ecs/component/component_type.h>
#include <global_context.h>

namespace rvr::system::ritual{
void Update(float dt) {
    for (auto entityId : GlobalContext::Inst()->GetECS()->GetEids(ComponentType::Ritual)) {
        auto ritual = GlobalContext::Inst()->GetECS()->GetComponent<Ritual>(entityId);
        if (ritual->canUpdate)
            ritual->Update(dt);
    }
}
void Begin() {
    for (auto entityId : GlobalContext::Inst()->GetECS()->GetEids(ComponentType::Ritual)) {
        auto ritual = GlobalContext::Inst()->GetECS()->GetComponent<Ritual>(entityId);
        ritual->Begin();
    }
}
}