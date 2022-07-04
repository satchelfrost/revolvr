#include "include/rvr_game_loop_timer.h"

RVRGameLoopTimer::RVRGameLoopTimer() {
    start_ = std::chrono::high_resolution_clock::now();
}

float RVRGameLoopTimer::RefreshDeltaTime() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = now - start_;
    start_ = now;
    return duration.count();
}