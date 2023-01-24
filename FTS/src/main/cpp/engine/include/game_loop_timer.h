#pragma once

#include <chrono>

namespace rvr {
class GameLoopTimer {
public:
    GameLoopTimer();
    void RefreshDeltaTime(float &dt);
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};
}