/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/vulkan_context.h>
#include <rendering/utilities/vulkan_results.h>
#include <platform/android_context.h>
#include <xr_context.h>
#include <global_context.h>
#include <ecs/system/spatial_system.h>
#include <math/linear_math.h>
#include <rendering/utilities/vulkan_utils.h>
#include <rendering/utilities/vertex_buffer_layout.h>
#include <rendering/utilities/vulkan_shader.h>

namespace rvr {
void VulkanContext::InitDevice(XrInstance xrInstance, XrSystemId systemId) {
    CheckVulkanGraphicsRequirements2KHR(xrInstance, systemId);
    CreateVulkanInstance(xrInstance, systemId);
    SetupReportCallback();
    PickPhysicalDevice(xrInstance, systemId);
    CreateLogicalDevice(xrInstance, systemId);
    CreateCommandPool();
    RetrieveQueues();
    StoreGraphicsBinding();
}

void VulkanContext::CreateVulkanInstance(XrInstance xrInstance, XrSystemId systemId) {
    // Optional application information
    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.pApplicationName = "app";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "RevolVR";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Vulkan instance information
    VkInstanceCreateInfo vkCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    vkCreateInfo.pApplicationInfo = &appInfo;
    std::vector<const char *> extensions = {"VK_EXT_debug_report"};
    bool validationLayerFound = CheckValidationLayerSupport();
    if (enableValidationLayers_ && validationLayerFound) {
        vkCreateInfo.enabledLayerCount = (uint32_t) requestedValidationLayers_.size();
        vkCreateInfo.ppEnabledLayerNames = requestedValidationLayers_.data();
        vkCreateInfo.enabledExtensionCount = (uint32_t) extensions.size();
        vkCreateInfo.ppEnabledExtensionNames = extensions.data();
    }
    else {
        vkCreateInfo.enabledLayerCount = 0;
        vkCreateInfo.ppEnabledLayerNames = nullptr;
        vkCreateInfo.enabledExtensionCount = (uint32_t) extensions.size();
        vkCreateInfo.ppEnabledExtensionNames = extensions.data();
    }

    // OpenXR-Specific Vulkan instance information
    XrVulkanInstanceCreateInfoKHR createInfo{XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR};
    createInfo.systemId = systemId;
    createInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
    createInfo.vulkanCreateInfo = &vkCreateInfo;
    createInfo.vulkanAllocator = nullptr;

    VkResult vkResult;
    XrResult xrResult;
    xrResult = CreateVulkanInstanceKHR(xrInstance, &createInfo, &instance_,
                                       &vkResult);
    CHECK_XRCMD(xrResult);
    CHECK_VKCMD(vkResult);
}

void VulkanContext::InitializeResources() {
    InitCubeResources();
    InitGltfResources();
}

XrSwapchainImageBaseHeader* VulkanContext::AllocateSwapchainImageStructs(
        uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo) {
    //

    auto context = std::make_shared<SwapchainImageContext>(renderingContext_,
                                                                         capacity, swapchainCreateInfo);
    auto images = context->GetFirstImagePointer();
    imageToSwapchainContext_.insert(std::make_pair(images, context));
    return images;
}

void VulkanContext::RenderView(const XrCompositionLayerProjectionView &layerView,
                               const XrSwapchainImageBaseHeader *swapchainImage, uint32_t imageIndex) {
    // Texture arrays not supported.
    CHECK(layerView.subImage.imageArrayIndex == 0);
    // Compute the view-projection transform.
    // Note all matrices (including OpenXR) are column-major, right-handed.
    glm::mat4 projectionMatrix = math::matrix::CreateProjectionFromXrFOV(layerView.fov, 0.05f, 100.0f);
    glm::mat4 poseMatrix = math::Pose(layerView.pose).ToMat4();
    glm::mat4 viewMatrix = glm::affineInverse(poseMatrix);
    glm::mat4 viewProjection = projectionMatrix * viewMatrix;
    std::vector<glm::mat4> mvps;
    renderBuffer_.clear();
    system::render::PopulateRenderTransformBuffer(renderBuffer_);
    for (const math::Transform &model : renderBuffer_) {
        glm::mat4 modelMatrix = model.ToMat4();
        glm::mat4 mvp = viewProjection * modelMatrix;
        mvps.push_back(mvp);
    }

    // Hardcode the shrek transforms for now until the models are hooked up to scene description
    std::vector<glm::mat4> mats;
    for (int i = 0; i < 4; i++) {
        math::Transform transform{};
        transform.SetPosition((float)i, 0, 0);
        transform.SetOrientation(math::quaternion::FromEuler(-90, 0, 0));
        mats.push_back(transform.ToMat4());
    }

    // Update uniform buffer
    uboScene.projection = projectionMatrix;
    uboScene.view = viewMatrix;
    auto position = math::Pose(layerView.pose).GetPosition();
    uboScene.viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
    uniformBuffer_->WriteToBuffer(&uboScene);

    auto swapchainContext = imageToSwapchainContext_[swapchainImage];
    swapchainContext->BeginRenderPass(imageIndex);
    swapchainContext->Draw(cubePipeline_, drawBuffer_, mvps);
    swapchainContext->DrawGltf(gltfPipeline_, model_, uboSceneDescriptorSet_, mats);
    swapchainContext->EndRenderPass();
}

std::vector<std::string> VulkanContext::GetInstanceExtensions() {
    return {XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME};
}

uint32_t VulkanContext::GetSupportedSwapchainSampleCount(const XrViewConfigurationView &) {
    return VK_SAMPLE_COUNT_1_BIT;
}

const XrBaseInStructure* VulkanContext::GetGraphicsBinding() const {
    return reinterpret_cast<const XrBaseInStructure *>(&graphicsBinding_);
}

bool VulkanContext::CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : requestedValidationLayers_) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                PrintInfo("Requested validation layer found " + std::string(layerName));
                break;
            }
        }

        if (!layerFound) {
            PrintWarning("No validation layers found");
            return false;
        }
    }
    return true;
}

void VulkanContext::SetupReportCallback() {
    if (!enableValidationLayers_)
        return;

    VkDebugReportCallbackCreateInfoEXT createInfo{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                       VK_DEBUG_REPORT_WARNING_BIT_EXT |
                       VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                       VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                       VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    createInfo.pfnCallback = DebugReportCallback;
    createInfo.pUserData = this;
    CreateDebugReportCallbackEXT(instance_, &createInfo, nullptr, &debugReporter_);
}

void VulkanContext::Cleanup() {
    if (enableValidationLayers_)
        DestroyDebugReportCallbackEXT(instance_, debugReporter_, nullptr);
}

void VulkanContext::PickPhysicalDevice(XrInstance xrInstance, XrSystemId systemId) {
    XrVulkanGraphicsDeviceGetInfoKHR deviceGetInfo{XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR};
    deviceGetInfo.systemId = systemId;
    deviceGetInfo.vulkanInstance = instance_;
    CHECK_XRCMD(GetVulkanGraphicsDevice2KHR(xrInstance, &deviceGetInfo,
                                            &physicalDevice_));
}

void VulkanContext::CreateLogicalDevice(XrInstance xrInstance, XrSystemId systemId) {
    queueFamilyIndices_ = FindQueueFamilies(physicalDevice_);
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndices_.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // Create the device
    std::vector<const char *> deviceExtensions;
    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo deviceInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;
    deviceInfo.enabledExtensionCount = (uint32_t) deviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = nullptr;
    deviceInfo.pEnabledFeatures = &features;

    // OpenXr-Specific device create information
    XrVulkanDeviceCreateInfoKHR deviceCreateInfo{XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR};
    deviceCreateInfo.systemId = systemId;
    deviceCreateInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
    deviceCreateInfo.vulkanCreateInfo = &deviceInfo;
    deviceCreateInfo.vulkanPhysicalDevice = physicalDevice_;
    deviceCreateInfo.vulkanAllocator = nullptr;

    VkResult vkResult;
    XrResult xrResult;
    xrResult = CreateVulkanDeviceKHR(xrInstance, &deviceCreateInfo, &device_,
                                     &vkResult);
    CHECK_XRCMD(xrResult);
    CHECK_VKCMD(vkResult);
}

void VulkanContext::CreateCommandPool() {
    VkCommandPoolCreateInfo cmdPoolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndices_.graphicsFamily.value();
    CHECK_VKCMD(vkCreateCommandPool(device_, &cmdPoolInfo, nullptr,
                                    &graphicsPool_));
}

void VulkanContext::StoreGraphicsBinding() {
    graphicsBinding_.type = XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR;
    graphicsBinding_.instance = instance_;
    graphicsBinding_.physicalDevice = physicalDevice_;
    graphicsBinding_.device = device_;
    graphicsBinding_.queueFamilyIndex = queueFamilyIndices_.graphicsFamily.value();
    graphicsBinding_.queueIndex = 0;
}

void VulkanContext::RetrieveQueues() {
    vkGetDeviceQueue(device_, queueFamilyIndices_.graphicsFamily.value(), 0,
                     &graphicsQueue_);
}

void VulkanContext::SwapchainImagesReady(XrSwapchainImageBaseHeader *images) {
    auto context = imageToSwapchainContext_[images];
    context->InitRenderTargets();
}

void VulkanContext::InitRenderingContext(VkFormat colorFormat) {
    renderingContext_ = std::make_shared<RenderingContext>(physicalDevice_, device_,
                                                           graphicsQueue_, colorFormat,
                                                           graphicsPool_);
//    InitializeResources();
}

std::shared_ptr<RenderingContext> VulkanContext::GetRenderingContext() {
    return renderingContext_;
}

void VulkanContext::InitCubeResources() {
    auto vert = std::make_unique<VulkanShader>(device_,
                                                       "shaders/basic.vert.spv",
                                                       VulkanShader::Vertex);
    vert->PushConstant("Model View Projection Matrix", sizeof(glm::mat4));
    auto frag= std::make_unique<VulkanShader>(device_,
                                              "shaders/basic.frag.spv",
                                              VulkanShader::Fragment);
    cubeShaderStages_ = std::make_unique<ShaderStages>(device_, std::move(vert),
                                                     std::move(frag));
    VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.Push({0, DataType::F32, 3, "Position"});
    vertexBufferLayout.Push({1, DataType::F32, 3, "Color"});
    size_t sizeOfIndex = sizeof(Geometry::c_cubeIndices[0]);
    size_t sizeOfVertex = sizeof(Geometry::c_cubeVertices[0]);
    size_t indexCount = sizeof(Geometry::c_cubeIndices) / sizeOfIndex;
    size_t vertexCount = sizeof(Geometry::c_cubeVertices) / sizeOfVertex;
    auto indexBuffer = std::make_unique<VulkanBuffer>(renderingContext_,
                                                      sizeOfIndex, indexCount,
                                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                  MemoryType::HostVisible);
    auto vertexBuffer = std::make_unique<VulkanBuffer>(renderingContext_,
                                                       sizeOfVertex, vertexCount,
                                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                   MemoryType::HostVisible);
    drawBuffer_ = std::make_unique<DrawBuffer>(std::move(indexBuffer),
                                               std::move(vertexBuffer));
    drawBuffer_->UpdateIndices(Geometry::c_cubeIndices);
    drawBuffer_->UpdateVertices(Geometry::c_cubeVertices);
    cubePipeline_ = std::make_unique<Pipeline>(renderingContext_, cubeShaderStages_, vertexBufferLayout,
                                               VK_FRONT_FACE_CLOCKWISE);
}

void VulkanContext::InitGltfResources() {
    // Setup model and uniform buffer before setting up descriptors
    model_ = std::make_unique<VulkanGLTFModel>(renderingContext_, "gltf/shrek/scene.gltf");
    uniformBuffer_ = std::make_unique<VulkanBuffer>(renderingContext_, sizeof(uboScene),
                                                    1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                    MemoryType::HostVisible);
    uniformBuffer_->Map();
    uniformBuffer_->WriteToBuffer(&uboScene);
    SetupDescriptors();

    // Create the shader stages, add any push constants and/or descriptor set layouts
    auto vert = std::make_unique<VulkanShader>(device_,
                                               "shaders/basic_gltf.vert.spv",
                                               VulkanShader::Vertex);
    vert->PushConstant("Model primitive", sizeof(glm::mat4));
    vert->AddSetLayout(descriptorSetLayouts_["shrek_ubo"]->GetDescriptorSetLayout());
    auto frag = std::make_unique<VulkanShader>(device_,
                                               "shaders/basic_gltf.frag.spv",
                                               VulkanShader::Fragment);
    frag->AddSetLayout(descriptorSetLayouts_["shrek_texture"]->GetDescriptorSetLayout());
    gltfShaderStages_ = std::make_unique<ShaderStages>(device_, std::move(vert),
                                                       std::move(frag));

    // Setup vertex buffer layout, and use that along with shader stages to create a pipeline
    VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.Push({0, DataType::F32, 3, "Position"});
    vertexBufferLayout.Push({1, DataType::F32, 3, "Normal"});
    vertexBufferLayout.Push({2, DataType::F32, 2, "UV"});
    vertexBufferLayout.Push({3, DataType::F32, 3, "Color"});
    gltfPipeline_ = std::make_unique<Pipeline>(renderingContext_, gltfShaderStages_, vertexBufferLayout,
                                               VK_FRONT_FACE_COUNTER_CLOCKWISE);
}

void VulkanContext::SetupDescriptors() {
    if (!model_)
        THROW("Model not loaded");

    globalDescriptorPool_ = DescriptorPool::Builder(device_)
            .SetMaxSets(model_->GetNumImages() + 1)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, model_->GetNumImages())
            .Build();

    // Setup descriptor set layouts
    descriptorSetLayouts_["shrek_ubo"] = DescriptorSetLayout::Builder(device_)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        VK_SHADER_STAGE_VERTEX_BIT)
            .Build();

    descriptorSetLayouts_["shrek_texture"] = DescriptorSetLayout::Builder(device_)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build();

    auto bufferInfo = uniformBuffer_->DescriptorInfo();
    DescriptorWriter(*descriptorSetLayouts_["shrek_ubo"], *globalDescriptorPool_)
        .WriteBuffer(0, &bufferInfo)
        .Build(uboSceneDescriptorSet_);

    // Descriptor sets for materials
    for (auto& image : model_->GetImages()) {
        DescriptorWriter(*descriptorSetLayouts_["shrek_texture"], *globalDescriptorPool_)
                .WriteImage(0, &image.texture.descriptor)
                .Build(image.descriptorSet);
    }
}
}