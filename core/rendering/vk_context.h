/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

#include "rendering/gltf/gltf_model.h"
#include "vk-utils/shader_stages.h"
#include "vk-utils/draw_buffer.h"
#include "vk-utils/pipeline.h"
#include "vk-utils/swapchain_image_context.h"
#include "vk-utils/rendering_context.h"
#include "vk-utils/descriptors.h"

#define MAX_LIGHTS 10

namespace rvr {
class XrContext;

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
    std::unique_ptr<DrawBuffer> drawBuffer_;
    std::unique_ptr<Pipeline> cubePipeline_;

    // Gltf stuff
    bool usingGltf_ = false;
    std::unique_ptr<ShaderStages> gltfShaderStages_;
    std::unique_ptr<Pipeline> gltfPipeline_;
    std::map<std::string, std::unique_ptr<GLTFModel>> models_;
    std::map<const XrSwapchainImageBaseHeader*, std::vector<std::shared_ptr<Buffer>>> imgToUniformBuffs_;
    std::map<const XrSwapchainImageBaseHeader*, std::vector<VkDescriptorSet>> imgToUboDescriptorSets_;
    struct PointLightData {
        glm::vec4 position; // ignore w
        glm::vec4 color;    // color + intensity
    };
    struct UBOScene {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec4 viewPos;
        glm::vec4 ambientColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.15f); // color + intensity
        PointLightData pointLights[MAX_LIGHTS];
        int numLights;
    } uboScene;
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
