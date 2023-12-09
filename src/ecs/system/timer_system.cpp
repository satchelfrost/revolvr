/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/system/timer_system.h>
#include <ecs/component/types/timer.h>
#include <global_context.h>

namespace rvr::sys::timer {
void UpdateTicks() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Timer);
    for (auto& [eid, component] : components) {
        auto timer = dynamic_cast<Timer*>(component);
        if (timer)
            timer->Tick();
        else
            PrintWarning("No timer component found");
    }
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
