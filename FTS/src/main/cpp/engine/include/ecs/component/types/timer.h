#pragma once

#include <chrono>
#include <ecs/component/component.h>

namespace rvr {
class Timer : public Component {
public:
    Timer(const Timer& other) = delete;
    Timer(const Timer& other, type::EntityId newEntityId);

    Timer(type::EntityId pId, std::chrono::high_resolution_clock::duration waitTime);
    virtual Component* Clone(type::EntityId newEntityId) override;
    void Tick();
    void Start();
    void Stop();
private:
    void Timeout();
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
    bool ticking_;
    bool autoStart_;
    bool oneShot_;
    std::chrono::high_resolution_clock::duration waitTime_;

};
}
