/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <scene.h>

#include <global_context.h>

#include <array>
#include <cmath>

namespace rvr {
class App {
public:
    App();
    ~App();
    void Run(struct android_app* app);

    float deltaTime_;
    Scene scene_;
    GlobalContext* globalContext_;

    bool requestRestart_; // TODO: do something with this variable
    bool exitRenderLoop_; // TODO: do something with this variable
};
}