/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <audio/audio_engine.h>
#include <rendering/vk_context.h>
#include <ecs/ecs.h>
#include <platform/android_context.h>
#include <xr_context.h>
#include "extension_manager.h"

namespace rvr {
class GlobalContext {
public:
    ~GlobalContext();
    static GlobalContext* Inst();
    void Init(android_app *app, std::vector<RVRExtensions> exts);

    void UpdateSystems(float deltaTime);

    AudioEngine* GetAudioEngine();
    XrContext* GetXrContext();
    VulkanContext* GetVulkanContext();
    ECS* GetECS();
    AndroidContext* GetAndroidContext();
    ExtensionManager* ExtMan();

    // Default value for player id
    type::EntityId PLAYER_ID = 0;
private:
    static GlobalContext* instance_;

    // Contextual data
    AudioEngine* audioEngine_;
    VulkanContext* vulkanRenderer_;
    AndroidContext* androidContext_;
    ECS* ecs_;
    XrContext* xrContext_;
    ExtensionManager* extensionManager_;

    bool initialized_ = false;
};
}