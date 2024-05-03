/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include "scene/scene.h"
#include "extension_manager.h"

#include <global_context.h>

#include <array>
#include <cmath>

namespace rvr {
class App {
public:
    App(std::vector<RVRExtensions> exts = {});
    ~App();
    void Run(struct android_app* app, const std::string& defaultScene);

    float deltaTime_;
    Scene scene_;
    GlobalContext* globalContext_;

    bool requestRestart_;
    bool exitRenderLoop_;

private:
    std::vector<RVRExtensions> exts_;
};
}