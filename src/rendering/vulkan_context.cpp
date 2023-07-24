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

namespace rvr {
void VulkanContext::Init(XrInstance xrInstance, XrSystemId systemId) {
    XrGraphicsRequirementsVulkan2KHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR};
    CheckVulkanGraphicsRequirements2KHR(xrInstance, systemId, &graphicsRequirements);
    CreateVulkanInstance(xrInstance, systemId);
    SetupReportCallback();
}

void VulkanContext::CreateVulkanInstance(XrInstance xrInstance, XrSystemId systemId) {
    bool validationLayerFound = CheckValidationLayerSupport();
    if (enableValidationLayers_ && !validationLayerFound)
        PrintWarning("Validation layers requested but not found");

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

void VulkanContext::InitializeDevice(XrInstance instance, XrSystemId systemId) {
    XrGraphicsRequirementsVulkan2KHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR};
    CheckVulkanGraphicsRequirements2KHR(instance, systemId, &graphicsRequirements);

    std::vector<const char *> layers;
//    #if !defined(NDEBUG)
//    const char *const validationLayerName = GetValidationLayerName();
//    if (validationLayerName)
//        layers.push_back(validationLayerName);
//    else
//        PrintWarning("No validation layers found in the system, skipping");
//    #endif

    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.pApplicationName = "fts";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "fts";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char *> extensions = {"VK_EXT_debug_report"};
    VkInstanceCreateInfo instInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledLayerCount = (uint32_t) layers.size();
    instInfo.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();
    instInfo.enabledExtensionCount = (uint32_t) extensions.size();
    instInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();

    XrVulkanInstanceCreateInfoKHR createInfo{XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR};
    createInfo.systemId = systemId;
    createInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
    createInfo.vulkanCreateInfo = &instInfo;
    createInfo.vulkanAllocator = nullptr;

    VkResult err;
    CHECK_XRCMD(CreateVulkanInstanceKHR(instance, &createInfo, &instance_, &err));
    CHECK_VKCMD(err);

    vkCreateDebugReportCallbackEXT_ =
            (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT_ =
            (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance_, "vkDestroyDebugReportCallbackEXT");

    VkDebugReportCallbackCreateInfoEXT debugInfo{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
#if !defined(NDEBUG)
    debugInfo.flags |=
            VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
            VK_DEBUG_REPORT_DEBUG_BIT_EXT;
#endif
    debugInfo.pfnCallback = DebugReportCallback;
    debugInfo.pUserData = this;
    CHECK_VKCMD(vkCreateDebugReportCallbackEXT_(instance_, &debugInfo, nullptr, &debugReporter_));

    XrVulkanGraphicsDeviceGetInfoKHR deviceGetInfo{XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR};
    deviceGetInfo.systemId = systemId;
    deviceGetInfo.vulkanInstance = instance_;
    CHECK_XRCMD(GetVulkanGraphicsDevice2KHR(instance, &deviceGetInfo, &physicalDevice_));

    VkDeviceQueueCreateInfo queueInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    float queuePriorities = 0;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriorities;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount,
                                             nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount,
                                             &queueFamilyProps[0]);

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        // Only need graphics (not presentation) for draw queue
        if ((queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u) {
            queueFamilyIndex_ = queueInfo.queueFamilyIndex = i;
            break;
        }
    }

    std::vector<const char *> deviceExtensions;
    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo deviceInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;
    deviceInfo.enabledExtensionCount = (uint32_t) deviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data();
    deviceInfo.pEnabledFeatures = &features;

    XrVulkanDeviceCreateInfoKHR deviceCreateInfo{XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR};
    deviceCreateInfo.systemId = systemId;
    deviceCreateInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
    deviceCreateInfo.vulkanCreateInfo = &deviceInfo;
    deviceCreateInfo.vulkanPhysicalDevice = physicalDevice_;
    deviceCreateInfo.vulkanAllocator = nullptr;

    CHECK_XRCMD(CreateVulkanDeviceKHR(instance, &deviceCreateInfo, &device_, &err));
    CHECK_VKCMD(err);

    vkGetDeviceQueue(device_, queueInfo.queueFamilyIndex, 0, &graphicsQueue_);
    memAllocator_.Init(physicalDevice_, device_);
    InitializeResources();

    graphicsBinding_.type = XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR;
    graphicsBinding_.instance = instance_;
    graphicsBinding_.physicalDevice = physicalDevice_;
    graphicsBinding_.device = device_;
    graphicsBinding_.queueFamilyIndex = queueInfo.queueFamilyIndex;
    graphicsBinding_.queueIndex = 0;
}

void VulkanContext::InitializeResources() {
    auto fragmentSPIRV = CreateSPIRVVector("shaders/basic.frag.spv");
    auto vertexSPIRV = CreateSPIRVVector("shaders/basic.vert.spv");

    if (vertexSPIRV.empty()) THROW("Failed to compile vertex shader");
    if (fragmentSPIRV.empty()) THROW("Failed to compile fragment shader");

    shaderProgram_.Init(device_);
    shaderProgram_.LoadVertexShader(vertexSPIRV);
    shaderProgram_.LoadFragmentShader(fragmentSPIRV);

    // Semaphore to block on draw complete
    VkSemaphoreCreateInfo semInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    CHECK_VKCMD(vkCreateSemaphore(device_, &semInfo, nullptr, &drawDone_));

    if (!cmdBuffer_.Init(device_, queueFamilyIndex_))
        THROW("Failed to create command buffer");

    pipelineLayout_.Create(device_);

    static_assert(sizeof(Geometry::Vertex) == 24, "Unexpected Vertex size");
    drawBuffer_.Init(device_, &memAllocator_,
                     {{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Geometry::Vertex, Position)},
                       {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Geometry::Vertex, Color)}});
    uint32_t numCubeIndices = sizeof(Geometry::c_cubeIndices) / sizeof(Geometry::c_cubeIndices[0]);
    uint32_t numCubeVertices =
            sizeof(Geometry::c_cubeVertices) / sizeof(Geometry::c_cubeVertices[0]);
    drawBuffer_.Create(numCubeIndices, numCubeVertices);
    drawBuffer_.UpdateIndices(Geometry::c_cubeIndices, numCubeIndices, 0);
    drawBuffer_.UpdateVertices(Geometry::c_cubeVertices, numCubeVertices, 0);
}

std::vector<XrSwapchainImageBaseHeader *> VulkanContext::AllocateSwapchainImageStructs(
        uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo) {
    // Allocate and initialize the buffer of image structs (must be sequential in memory for xrEnumerateSwapchainImages).
    // Return back an array of pointers to each swapchain image struct so the consumer doesn't need to know the type/size.
    // Keep the buffer alive by adding it into the list of buffers.
    swapchainImageContexts_.emplace_back(GetSwapchainImageType());

    SwapchainImageContext &swapchainImageContext = swapchainImageContexts_.back();

    std::vector<XrSwapchainImageBaseHeader *> bases = swapchainImageContext.Create(
            device_, &memAllocator_, capacity, swapchainCreateInfo, pipelineLayout_,
            shaderProgram_, drawBuffer_);

    // Map every swapchainImage base pointer to this context
    for (auto &base : bases) {
        swapchainImageContextMap_[base] = &swapchainImageContext;
    }

    return bases;
}

void VulkanContext::RenderView(const XrCompositionLayerProjectionView &layerView,
                               const XrSwapchainImageBaseHeader *swapchainImage,
                               int64_t /*swapchainFormat*/, const std::vector<math::Transform> &cubes) {
    CHECK(layerView.subImage.imageArrayIndex == 0);  // Texture arrays not supported.

    auto swapchainContext = swapchainImageContextMap_[swapchainImage];
    uint32_t imageIndex = swapchainContext->ImageIndex(swapchainImage);

    cmdBuffer_.Reset();
    cmdBuffer_.Begin();

    // Ensure depth is in the right layout
    swapchainContext->depthBuffer.
    TransitionLayout(&cmdBuffer_, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // Bind and clear eye render target
    static XrColor4f darkSlateGrey = {0.184313729f, 0.309803933f, 0.309803933f, 1.0f};
    static std::array<VkClearValue, 2> clearValues;
    clearValues[0].color.float32[0] = darkSlateGrey.r;
    clearValues[0].color.float32[1] = darkSlateGrey.g;
    clearValues[0].color.float32[2] = darkSlateGrey.b;
    clearValues[0].color.float32[3] = darkSlateGrey.a;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;
    VkRenderPassBeginInfo renderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassBeginInfo.clearValueCount = (uint32_t) clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();

    swapchainContext->BindRenderTarget(imageIndex, &renderPassBeginInfo);

    vkCmdBeginRenderPass(cmdBuffer_.buf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdBuffer_.buf, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchainContext->pipe.pipe);

    // Bind index and vertex buffers
    vkCmdBindIndexBuffer(cmdBuffer_.buf, drawBuffer_.idxBuf, 0, VK_INDEX_TYPE_UINT16);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmdBuffer_.buf, 0, 1, &drawBuffer_.vtxBuf, &offset);

    // Compute the view-projection transform.
    // Note all matrices (including OpenXR) are column-major, right-handed.
    const auto &pose = layerView.pose;

    glm::mat4 projectionMatrix = math::matrix::CreateProjectionFromXrFOV(layerView.fov, 0.05f, 100.0f);
    glm::mat4 poseMatrix = math::Pose(pose).ToMat4();
    glm::mat4 viewMatrix = glm::affineInverse(poseMatrix);
    glm::mat4 viewProjection = projectionMatrix * viewMatrix;

    // Render each cube
    for (const math::Transform &cube : cubes) {
        glm::mat4 modelMatrix = cube.ToMat4();
        glm::mat4 mvp = viewProjection * modelMatrix;
        vkCmdPushConstants(cmdBuffer_.buf, pipelineLayout_.layout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvp),
                           glm::value_ptr(mvp));
        vkCmdDrawIndexed(cmdBuffer_.buf, drawBuffer_.count.idx,
                         1, 0, 0, 0);
    }

    vkCmdEndRenderPass(cmdBuffer_.buf);

    cmdBuffer_.End();
    cmdBuffer_.Exec(graphicsQueue_);

    // XXX Should double-buffer the command buffers, for now just flush
    cmdBuffer_.Wait();
}

void VulkanContext::Render() {
    XrContext* xrContext = GlobalContext::Inst()->GetXrContext();
    if (xrContext->frameState.shouldRender == XR_TRUE) {
        if (RenderLayer(xrContext->projectionLayerViews, xrContext->mainLayer, xrContext)) {
            xrContext->AddMainLayer();
        }
    }
}

bool VulkanContext::RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                                XrCompositionLayerProjection& layer, XrContext* xrContext) {
    XrViewState viewState{XR_TYPE_VIEW_STATE};
    auto viewCapacityInput = (uint32_t)xrContext->views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = xrContext->viewConfigType;
    viewLocateInfo.displayTime = xrContext->frameState.predictedDisplayTime;
    viewLocateInfo.space = xrContext->appSpace;

    XrResult res = xrLocateViews(xrContext->session, &viewLocateInfo, &viewState, viewCapacityInput,
                                 &viewCountOutput, xrContext->views.data());
    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == xrContext->configViews.size());
    CHECK(viewCountOutput == xrContext->swapchains.size());

    projectionLayerViews.resize(viewCountOutput);

    // Convert renderable to a cube for now
    for (auto spatial : system::render::GetRenderSpatials()) {
        math::Transform cube{};
        cube = system::spatial::GetPlayerRelativeTransform(spatial);
        renderBuffer_.push_back(cube);
    }

    // Draw a simple grid
    DrawGrid();

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < viewCountOutput; i++) {
        // Each view has a separate swapchain which is acquired, rendered to, and released.
        const Swapchain viewSwapchain = xrContext->swapchains[i];

        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

        uint32_t swapchainImageIndex;
        CHECK_XRCMD(xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex));

        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        CHECK_XRCMD(xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo));

        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        projectionLayerViews[i].pose = xrContext->views[i].pose;
        projectionLayerViews[i].fov = xrContext->views[i].fov;
        projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {viewSwapchain.width, viewSwapchain.height};

        const XrSwapchainImageBaseHeader* const swapchainImage = xrContext->swapchainImageMap[viewSwapchain.handle][swapchainImageIndex];
        RenderView(projectionLayerViews[i], swapchainImage, xrContext->colorSwapchainFormat, renderBuffer_);

        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        CHECK_XRCMD(xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo));
    }

    // Clear the renderBuffer for the next frame
    renderBuffer_.clear();

    layer.space = xrContext->appSpace;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();
    return true;
}

void VulkanContext::DrawGrid() {
    // Get the player and spatial world transforms
    auto player = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(GlobalContext::Inst()->PLAYER_ID);
    CHECK_MSG(player, "Player spatial does not exist inside of GetPlayerRelativeTransform()");
    math::Transform playerWorld = player->GetWorld();
    auto invOrientation = glm::inverse(playerWorld.GetOrientation());

    // Draw the horizontal lines
    for (int i = -5; i <= 5; i ++) {
        math::Transform cube;
        glm::vec3 position(0, 0, (float)i * 2.0f);
        auto relativePosition = position - playerWorld.GetPosition();
        cube.SetPosition(invOrientation * relativePosition);
        cube.SetOrientation(invOrientation);
        cube.SetScale(20.0f, 0.1f, 0.1f);
        renderBuffer_.push_back(cube);
    }
    // Draw the vertical lines
    for (int i = -5; i <= 5; i ++) {
        math::Transform cube;
        glm::vec3 position((float)i * 2.0f, 0, 0);
        auto relativePosition = position - playerWorld.GetPosition();
        cube.SetPosition(invOrientation * relativePosition);
        cube.SetOrientation(invOrientation);
        cube.SetScale(0.1f, 0.1f, 20.0f);
        renderBuffer_.push_back(cube);
    }
}

std::vector<std::string> VulkanContext::GetInstanceExtensions() {
    return {XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME};
}

XrStructureType VulkanContext::GetSwapchainImageType() {
    return XR_TYPE_SWAPCHAIN_IMAGE_VULKAN2_KHR;
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

    PrintVerbose("Checking for validation layers");
    for (const char* layerName : requestedValidationLayers_) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                PrintVerbose("Requested validation layer found");
                break;
            }
        }

        if (!layerFound) {
            PrintVerbose("Requested validation layer was not found");
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
    if (enableValidationLayers_) {
        DestroyDebugReportCallbackEXT(instance_, debugReporter_, nullptr);
    }
}
}