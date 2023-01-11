#pragma once

#include <chrono>

class RVRGameLoopTimer {
public:
    RVRGameLoopTimer();
    void RefreshDeltaTime(float &dt);
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};