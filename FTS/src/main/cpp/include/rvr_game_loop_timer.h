#pragma once

#include <chrono>

class RVRGameLoopTimer {
public:
    RVRGameLoopTimer();
    float RefreshDeltaTime();
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};