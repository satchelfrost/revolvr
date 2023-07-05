/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

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
