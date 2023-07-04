/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <chrono>
#include <ecs/component/component.h>

namespace rvr {
class Timer : public Component {
public:
    Timer(type::EntityId pId, bool autostart, bool oneShot, std::chrono::high_resolution_clock::duration waitTime);
    Timer(const Timer& other) = delete;
    Timer(const Timer& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    bool IsAutoStart() const;
    void Tick();
    void Start();
    void Stop();
private:
    void Timeout();
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
    bool ticking_;

    // user variables
    bool autoStart_;
    bool oneShot_;
    std::chrono::high_resolution_clock::duration waitTime_;

};
}
