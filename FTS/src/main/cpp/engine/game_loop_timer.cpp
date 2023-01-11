#include "rvr_game_loop_timer.h"

RVRGameLoopTimer::RVRGameLoopTimer() {
    start_ = std::chrono::high_resolution_clock::now();
}

void RVRGameLoopTimer::RefreshDeltaTime(float &dt) {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = now - start_;
    start_ = now;
    dt = duration.count();
}