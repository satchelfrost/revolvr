#include <game_loop_timer.h>

namespace rvr {
GameLoopTimer::GameLoopTimer() {
    start_ = std::chrono::high_resolution_clock::now();
}

void GameLoopTimer::RefreshDeltaTime(float &dt) {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = now - start_;
    start_ = now;
    dt = duration.count();
}
}