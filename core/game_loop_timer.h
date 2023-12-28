/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

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