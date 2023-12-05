/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

// TODO: check unused includes
#include "rendering/utilities/shader_stages.h"
#include "rendering/utilities/draw_buffer.h"
#include "rendering/utilities/pipeline.h"
#include "rendering/utilities/vulkan_utils.h"
#include <rendering/utilities/geometry.h>
#include <rendering/utilities/swapchain_image_context.h>
#include <rendering/utilities/rendering_context.h>
#include <platform/android_context.h>
#include <ecs/system/render_system.h>
#include <rendering/utilities/gltf/vulkan_gltf_model.h>
#include <rendering/utilities/vulkan_descriptors.h>

#define MAX_LIGHTS 10

namespace rvr {
class XrContext;

struct PointLightData {
    glm::vec4 position; // ignore w
    glm::vec4 color;    // color + intensity
};

struct UBOScene {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec4 viewPos;
    glm::vec4 ambientColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.05f); // color + intensity
    PointLightData pointLights[MAX_LIGHTS];
    int numLights;

    // Cel shading
    float outlineWidth = 0.01f;
    int numBands = 1;
    float h = 0.05f;
    float b0 = 0.5;
    float b1 = 0.5;
    float b2 = 0.75;
};

class VulkanContext {
private:
    VkInstance instance_{VK_NULL_HANDLE};
    VkDebugReportCallbackEXT debugReporter_{VK_NULL_HANDLE};
    const std::vector<const char*> requestedValidationLayers_ = {"VK_LAYER_KHRONOS_validation"};
    VkPhysicalDevice physicalDevice_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};
    VkQueue graphicsQueue_{VK_NULL_HANDLE};
    VkCommandPool graphicsPool_;
    std::map<const XrSwapchainImageBaseHeader*, std::shared_ptr<SwapchainImageContext>> imageToSwapchainContext_;
    std::vector<math::Transform> renderBuffer_;
    XrGraphicsBindingVulkan2KHR graphicsBinding_{XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR};
    std::shared_ptr<RenderingContext> renderingContext_;
    QueueFamilyIndices queueFamilyIndices_;

    // Cube stuff
    std::unique_ptr<ShaderStages> cubeShaderStages_;
    std::unique_ptr<DrawBuffer> cubeBuffer_;
    std::unique_ptr<Pipeline> cubePipeline_;


    // Orbiting Lights pipeline
    std::unique_ptr<Pipeline> lightsPipeline_;
    std::vector<PointLightPushConst> lightPushConstInfo_;
    std::unique_ptr<DrawBuffer> lightBuffer_;
    std::unique_ptr<ShaderStages> lightShaderStages_;

    // Gltf stuff
    bool usingGltf_ = false;
    std::unique_ptr<ShaderStages> gltfShaderStages_;
    std::unique_ptr<ShaderStages> outlineShaderStages_;
    std::unique_ptr<Pipeline> gltfPipeline_;
    std::unique_ptr<Pipeline> outlinePipeline_;
    std::map<std::string, std::unique_ptr<VulkanGLTFModel>> models_;
    std::unique_ptr<VulkanBuffer> uniformBuffer_;
    VkDescriptorSet uboSceneDescriptorSet_;
    UBOScene uboScene;
    std::unique_ptr<DescriptorPool> globalDescriptorPool_;
    std::map<std::string, std::unique_ptr<DescriptorSetLayout>> descriptorSetLayouts_;

    // Point cloud stuff
    std::unique_ptr<ShaderStages> pointCloudShaderStages_;
    std::map<std::string, std::unique_ptr<PointCloudResource>> pointClouds_;
    std::unique_ptr<Pipeline> pointCloudPipeline_;


#if !defined(NDEBUG)
        const bool enableValidationLayers_ = true;
#else
        const bool enableValidationLayers_ = false;
#endif

public:
    UBOScene GetUniform() const;
    void SetUniform(UBOScene scene);
    void InitDevice(XrInstance xrInstance, XrSystemId systemId);
    void Cleanup();
    XrSwapchainImageBaseHeader* AllocateSwapchainImageStructs(uint32_t capacity,
                                                              const XrSwapchainCreateInfo& swapchainCreateInfo);
    static std::vector<std::string> GetInstanceExtensions();
    static uint32_t GetSupportedSwapchainSampleCount(const XrViewConfigurationView& view);
    const XrBaseInStructure* GetGraphicsBinding() const;
    void InitializeResources();
    void InitRenderingContext(VkFormat colorFormat);
    void SwapchainImagesReady(XrSwapchainImageBaseHeader* images);
    void RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage,
                    const uint32_t imageIndex);

private:
    void SetupDescriptors();
    void InitCubeResources();
    void InitGltfResources();
    void InitLightsResources();
    void InitPointCloudResources();
    void CreateVulkanInstance(XrInstance xrInstance, XrSystemId systemId);
    bool CheckValidationLayerSupport();
    void SetupReportCallback();
    void PickPhysicalDevice(XrInstance xrInstance, XrSystemId systemId);
    void CreateLogicalDevice(XrInstance xrInstance, XrSystemId systemId);
    void CreateCommandPool();
    void StoreGraphicsBinding();
    void RetrieveQueues();
};
}