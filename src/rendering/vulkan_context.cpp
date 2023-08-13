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
    auto fragmentSPIRV = CreateSPIRVVector("shaders/basic.frag.spv");
    auto vertexSPIRV = CreateSPIRVVector("shaders/basic.vert.spv");

    if (vertexSPIRV.empty()) THROW("Failed to compile vertex shader");
    if (fragmentSPIRV.empty()) THROW("Failed to compile fragment shader");

    shaderProgram_.Init(device_);
    shaderProgram_.LoadVertexShader(vertexSPIRV);
    shaderProgram_.LoadFragmentShader(fragmentSPIRV);
    drawBuffer_ = std::make_shared<DrawBuffer>(renderingContext_,
                                               sizeof(Geometry::c_cubeIndices[0]),
                                               sizeof(Geometry::c_cubeIndices),
                                               sizeof(Geometry::c_cubeVertices[0]),
                                               sizeof(Geometry::c_cubeVertices));
    drawBuffer_->UpdateIndices(Geometry::c_cubeIndices);
    drawBuffer_->UpdateVertices(Geometry::c_cubeVertices);
    pipeline_ = std::make_shared<Pipeline>(renderingContext_, shaderProgram_, drawBuffer_);
}

XrSwapchainImageBaseHeader* VulkanContext::AllocateSwapchainImageStructs(
        uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo) {
    auto context = std::make_shared<SwapchainImageContext>(renderingContext_,
                                                                         capacity, swapchainCreateInfo);
    auto images = context->GetFirstImagePointer();
    imageToSwapchainContext_.insert(std::make_pair(images, context));
    return images;
}

void VulkanContext::RenderView(const XrCompositionLayerProjectionView &layerView,
                               const XrSwapchainImageBaseHeader *swapchainImage,
                               uint32_t imageIndex, std::vector<math::Transform> &models) {
    CHECK(layerView.subImage.imageArrayIndex == 0);  // Texture arrays not supported.
    // Compute the view-projection transform.
    // Note all matrices (including OpenXR) are column-major, right-handed.
//    const auto &pose = layerView.pose;

    glm::mat4 projectionMatrix = math::matrix::CreateProjectionFromXrFOV(layerView.fov, 0.05f, 100.0f);
    glm::mat4 poseMatrix = math::Pose(layerView.pose).ToMat4();
    glm::mat4 viewMatrix = glm::affineInverse(poseMatrix);
    glm::mat4 viewProjection = projectionMatrix * viewMatrix;
    for (math::Transform &model : models) {
        model = viewProjection * model.ToMat4();
//        glm::mat4 modelMatrix = transform.ToMat4();
//        glm::mat4 mvp = viewProjection * modelMatrix;
//        transform = math::Transform(mvp);
    }

    auto swapchainContext = imageToSwapchainContext_[swapchainImage];
    // TODO: index count parameter is incorrect
    swapchainContext->Draw(imageIndex, models.size(), pipeline_, models);
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

        const XrSwapchainImageBaseHeader* const swapchainImage = xrContext->swapchainImages_[viewSwapchain.handle];
        RenderView(projectionLayerViews[i], swapchainImage, swapchainImageIndex, renderBuffer_);

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
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_);
    graphicsBinding_.type = XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR;
    graphicsBinding_.instance = instance_;
    graphicsBinding_.physicalDevice = physicalDevice_;
    graphicsBinding_.device = device_;
    graphicsBinding_.queueFamilyIndex = indices.graphicsFamily.value();
    graphicsBinding_.queueIndex = 0;
}

void VulkanContext::RetrieveQueues() {
    vkGetDeviceQueue(device_, queueFamilyIndices_.graphicsFamily.value(),
                     0, &graphicsQueue_);
}

void VulkanContext::SwapchainImagesReady(XrSwapchainImageBaseHeader *images) {
    auto context = imageToSwapchainContext_[images];
    context->InitRenderTargets();
}

void VulkanContext::InitRenderingContext(VkFormat colorFormat) {
    renderingContext_ = std::make_shared<RenderingContext>(physicalDevice_, device_,
                                                           graphicsQueue_, colorFormat,
                                                           graphicsPool_);
    InitializeResources();
}
}