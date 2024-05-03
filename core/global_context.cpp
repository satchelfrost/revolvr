/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <global_context.h>
#include <common.h>
#include <ecs/system/spatial_system.h>
#include <ecs/system/render_system.h>
#include <ecs/system/collision_system.h>
#include <ecs/system/ritual_system.h>
#include <ecs/system/timer_system.h>

namespace rvr {
GlobalContext* GlobalContext::instance_ = nullptr;

GlobalContext* GlobalContext::Inst() {
    if (!instance_)
        instance_ = new GlobalContext();
    return instance_;
}

void GlobalContext::Init(android_app *app, std::vector<RVRExtensions> exts) {
    if (initialized_)
        THROW("Cannot initialize Global Context twice");
    initialized_ = true;

    // Order matters here
    extensionManager_ = new ExtensionManager(exts);
    ecs_ = new ECS();
    audioEngine_ = new AudioEngine();
    androidContext_ = new AndroidContext(app);
    vulkanRenderer_ = new VulkanContext();
    xrContext_ = new XrContext();
}

GlobalContext::~GlobalContext() {
    delete audioEngine_;
    delete vulkanRenderer_;
    delete ecs_;
    delete xrContext_;
    delete androidContext_;
}

AudioEngine *GlobalContext::GetAudioEngine() {
    CHECK_MSG(initialized_, "Global Context was not initialized")
    return audioEngine_;
}

void GlobalContext::UpdateSystems(float deltaTime) {
    CHECK_MSG(initialized_, "Global Context was not initialized")
    sys::spatial::UpdateTrackedSpaces(xrContext_);
    sys::collision::RunCollisionChecks();
    sys::ritual::Update(deltaTime);
    sys::timer::UpdateTicks();
    sys::spatial::UpdateSpatials();
}

VulkanContext *GlobalContext::GetVulkanContext() {
    CHECK_MSG(initialized_, "Global Context was not initialized")
    return vulkanRenderer_;
}

XrContext *GlobalContext::GetXrContext() {
    CHECK_MSG(initialized_, "Global Context was not initialized")
    return xrContext_;
}

ECS *GlobalContext::GetECS() {
    CHECK_MSG(initialized_, "Global Context was not initialized")
    return ecs_;
}

AndroidContext *GlobalContext::GetAndroidContext() {
    CHECK_MSG(initialized_, "Global Context was not initialized")
    return androidContext_;
}

ExtensionManager* GlobalContext::ExtMan() {
    CHECK_MSG(initialized_, "Global Context was not initialized")
    return extensionManager_;
}
}