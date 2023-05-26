#include <ecs/component/types/timer.h>
#include <global_context.h>
#include <ecs/component/types/ritual.h>

namespace rvr {
Timer::Timer(type::EntityId pId, bool autostart, bool oneShot, std::chrono::high_resolution_clock::duration waitTime) :
Component(ComponentType::Timer, pId),
autoStart_(autostart), oneShot_(oneShot), ticking_(false), waitTime_(waitTime) {}

Timer::Timer(const Timer& other, type::EntityId newEntityId) :
Component(ComponentType::Timer, newEntityId),
ticking_(other.ticking_), autoStart_(other.autoStart_), oneShot_(other.oneShot_), waitTime_(other.waitTime_) {}

Component *Timer::Clone(type::EntityId newEntityId) {
    return new Timer(*this, newEntityId);
}

void Timer::Timeout() {
    auto entity = GlobalContext::Inst()->GetECS()->GetEntity(id);
    if (entity->HasComponent(ComponentType::Ritual)) {
        auto ritual = GlobalContext::Inst()->GetECS()->GetComponent<Ritual>(id);
        ritual->OnTimeout();
    }
    else {
        Log::Write(Log::Level::Warning,
                   Fmt("Timer component %s did not have associated ritual", entity->GetName().c_str()));
    }
}

void Timer::Tick() {
    if (ticking_) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - start_;
        if (waitTime_ < duration) {
            Timeout();
            if (oneShot_)
                ticking_ = false;
            else
                Start();
        }
    }
}

void Timer::Start() {
    ticking_ = true;
    start_ = std::chrono::high_resolution_clock::now();
}

void Timer::Stop() {
    ticking_ = false;
}

bool Timer::IsAutoStart() const {
    return autoStart_;
}
}
