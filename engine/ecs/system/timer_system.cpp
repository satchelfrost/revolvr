#include <ecs/system/timer_system.h>
#include <ecs/component/types/timer.h>
#include <global_context.h>

namespace rvr::system::timer {
void UpdateTicks() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Timer);
    for (auto& [eid, component] : components)
        reinterpret_cast<Timer*>(component)->Tick();
}
void Start() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Timer);
    for (auto& [eid, component] : components) {
        auto timer = reinterpret_cast<Timer*>(component);
        if (timer->IsAutoStart())
            timer->Start();
    }
}
}
