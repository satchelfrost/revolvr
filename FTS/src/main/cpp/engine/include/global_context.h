#pragma once

#include <audio/audio_engine.h>
#include <renderer/vulkan_renderer.h>
#include <ecs/ecs.h>
#include <platform/android_context.h>
#include <xr_context.h>

namespace rvr {
class GlobalContext {
public:
    ~GlobalContext();
    void Init(android_app *app);
    static GlobalContext* Inst();

    void UpdateSystems(float deltaTime);

    AudioEngine* GetAudioEngine();
    XrContext* GetXrContext();
    VulkanRenderer* GetVulkanRenderer();
    ECS* GetECS();
    AndroidContext* GetAndroidContext();

private:
    static GlobalContext* instance_;

    // Contextual data
    AudioEngine* audioEngine_;
    VulkanRenderer* vulkanRenderer_;
    AndroidContext* androidContext_;
    ECS* ecs_;
    XrContext* xrContext_;

    bool initialized_ = false;
};
}