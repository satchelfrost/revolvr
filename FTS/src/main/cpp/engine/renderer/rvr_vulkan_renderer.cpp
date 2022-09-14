#include "renderer/rvr_vulkan_renderer.h"
#include "platform/rvr_android_context.h"

extern "C" void fast_matrix_mul(float *, float *, float *);

RVRVulkanRenderer::RVRVulkanRenderer(const RVRAndroidContext* android_platform) {
    m_graphicsBinding.type = GetGraphicsBindingType();
};

std::vector<std::string> RVRVulkanRenderer::GetInstanceExtensions() const {
    return {XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME};
}

// Note: The output must not outlive the input - this modifies the input and returns a collection of views into that modified
// input!
std::vector<const char *> RVRVulkanRenderer::ParseExtensionString(char *names) {
    std::vector<const char *> list;
    while (*names != 0) {
        list.push_back(names);
        while (*(++names) != 0) {
            if (*names == ' ') {
                *names++ = '\0';
                break;
            }
        }
    }
    return list;
}

const char *RVRVulkanRenderer::GetValidationLayerName() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::vector<const char *> validationLayerNames;
    validationLayerNames.push_back("VK_LAYER_KHRONOS_validation");
    validationLayerNames.push_back("VK_LAYER_LUNARG_standard_validation");

    // Enable only one validation layer from the list above. Prefer KHRONOS.
    for (auto &validationLayerName: validationLayerNames) {
        for (const auto &layerProperties: availableLayers) {
            if (0 == strcmp(validationLayerName, layerProperties.layerName)) {
                return validationLayerName;
            }
        }
    }

    return nullptr;
}

void RVRVulkanRenderer::InitializeDevice(XrInstance instance, XrSystemId systemId) {
// Create the Vulkan device for the adapter associated with the system.
// Extension function must be loaded by name
    XrGraphicsRequirementsVulkan2KHR graphicsRequirements{
            XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR};
    CHECK_XRCMD(GetVulkanGraphicsRequirements2KHR(instance, systemId, &graphicsRequirements));

    VkResult err;

    std::vector<const char *> layers;
#if !defined(NDEBUG)
    const char *const validationLayerName = GetValidationLayerName();
    if (validationLayerName) {
        layers.
                push_back(validationLayerName);
    } else {
        Log::Write(Log::Level::Warning,
                   "No validation layers found in the system, skipping");
    }
#endif

    std::vector<const char *> extensions;
    extensions.push_back("VK_EXT_debug_report");
#if defined(USE_MIRROR_WINDOW)
    extensions.push_back("VK_KHR_surface");
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    extensions.push_back("VK_KHR_win32_surface");
#else
#error CreateSurface not supported on this OS
#endif  // defined(VK_USE_PLATFORM_WIN32_KHR)
#endif  // defined(USE_MIRROR_WINDOW)

    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.pApplicationName = "fts";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "fts";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;

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
    CHECK_XRCMD(CreateVulkanInstanceKHR(instance, &createInfo, &m_vkInstance, &err));
    CHECK_VKCMD(err);

    vkCreateDebugReportCallbackEXT =
            (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(m_vkInstance,"vkCreateDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT =
            (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(m_vkInstance,"vkDestroyDebugReportCallbackEXT");

    VkDebugReportCallbackCreateInfoEXT debugInfo{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
#if !defined(NDEBUG)
    debugInfo.flags |=
            VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
            VK_DEBUG_REPORT_DEBUG_BIT_EXT;
#endif
    debugInfo.pfnCallback = debugReportThunk;
    debugInfo.pUserData = this;
    CHECK_VKCMD(vkCreateDebugReportCallbackEXT(m_vkInstance, &debugInfo, nullptr, &m_vkDebugReporter));

    XrVulkanGraphicsDeviceGetInfoKHR deviceGetInfo{XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR};
    deviceGetInfo.systemId = systemId;
    deviceGetInfo.vulkanInstance = m_vkInstance;
    CHECK_XRCMD(GetVulkanGraphicsDevice2KHR(instance, &deviceGetInfo, &m_vkPhysicalDevice));

    VkDeviceQueueCreateInfo queueInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    float queuePriorities = 0;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriorities;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamilyCount,
                                             nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamilyCount,
                                             &queueFamilyProps[0]);

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        // Only need graphics (not presentation) for draw queue
        if ((queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u) {
            m_queueFamilyIndex = queueInfo.queueFamilyIndex = i;
            break;
        }
    }

    std::vector<const char *> deviceExtensions;

    VkPhysicalDeviceFeatures features{};
// features.samplerAnisotropy = VK_TRUE;

#if defined(USE_MIRROR_WINDOW)
    deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#endif

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
    deviceCreateInfo.vulkanPhysicalDevice = m_vkPhysicalDevice;
    deviceCreateInfo.vulkanAllocator = nullptr;
    CHECK_XRCMD(CreateVulkanDeviceKHR(instance, &deviceCreateInfo, &m_vkDevice, &err));
    CHECK_VKCMD(err);

    vkGetDeviceQueue(m_vkDevice, queueInfo.queueFamilyIndex, 0, &m_vkQueue);

    m_memAllocator.Init(m_vkPhysicalDevice, m_vkDevice);

    InitializeResources();

    m_graphicsBinding.instance = m_vkInstance;
    m_graphicsBinding.physicalDevice = m_vkPhysicalDevice;
    m_graphicsBinding.device = m_vkDevice;
    m_graphicsBinding.queueFamilyIndex = queueInfo.queueFamilyIndex;
    m_graphicsBinding.queueIndex = 0;
}

std::vector<char> RVRVulkanRenderer::CreateSPIRVVector(const char *asset_name) {
    // Load in the compiled shader from the apk
    AAsset *file = AAssetManager_open(RVRAndroidContext::GetInstance()->GetAndroidAssetManager(),
                                      asset_name,
                                      AASSET_MODE_BUFFER);
    off_t file_length = AAsset_getLength(file);
    char *file_content = new char[file_length];
    AAsset_read(file, file_content, file_length);
    std::vector<char> shader_vector(file_content, file_content + file_length);
    delete[] file_content;
    return shader_vector;
}

void RVRVulkanRenderer::InitializeResources() {
    auto fragmentSPIRV = CreateSPIRVVector("shaders/basic.frag.spv");
    auto vertexSPIRV = CreateSPIRVVector("shaders/basic.vert.spv");

    if (vertexSPIRV.empty()) THROW("Failed to compile vertex shader");
    if (fragmentSPIRV.empty()) THROW("Failed to compile fragment shader");

    m_shaderProgram.Init(m_vkDevice);
    m_shaderProgram.LoadVertexShader(vertexSPIRV);
    m_shaderProgram.LoadFragmentShader(fragmentSPIRV);

    // Semaphore to block on draw complete
    VkSemaphoreCreateInfo semInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    CHECK_VKCMD(vkCreateSemaphore(m_vkDevice, &semInfo, nullptr, &m_vkDrawDone));

    if (!m_cmdBuffer.Init(m_vkDevice, m_queueFamilyIndex))
        THROW("Failed to create command buffer");

    m_pipelineLayout.Create(m_vkDevice);

    static_assert(sizeof(Geometry::Vertex) == 24, "Unexpected Vertex size");
    m_drawBuffer.Init(m_vkDevice, &m_memAllocator,
                      {{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Geometry::Vertex, Position)},
                       {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Geometry::Vertex, Color)}});
    uint32_t numCubeIndices = sizeof(Geometry::c_cubeIndices) / sizeof(Geometry::c_cubeIndices[0]);
    uint32_t numCubeVertices =
            sizeof(Geometry::c_cubeVertices) / sizeof(Geometry::c_cubeVertices[0]);
    m_drawBuffer.Create(numCubeIndices, numCubeVertices);
    m_drawBuffer.UpdateIndices(Geometry::c_cubeIndices, numCubeIndices, 0);
    m_drawBuffer.UpdateVertices(Geometry::c_cubeVertices, numCubeVertices, 0);
}

int64_t RVRVulkanRenderer::SelectColorSwapchainFormat(const std::vector<int64_t> &runtimeFormats) const {
    // List of supported color swapchain formats.
    constexpr int64_t SupportedColorSwapchainFormats[] = {VK_FORMAT_B8G8R8A8_SRGB,
                                                          VK_FORMAT_R8G8B8A8_SRGB,
                                                          VK_FORMAT_B8G8R8A8_UNORM,
                                                          VK_FORMAT_R8G8B8A8_UNORM};

    auto swapchainFormatIt = std::find_first_of(runtimeFormats.begin(), runtimeFormats.end(),
                               std::begin(SupportedColorSwapchainFormats),
                               std::end(SupportedColorSwapchainFormats));
    if (swapchainFormatIt == runtimeFormats.end()) {
        THROW("No runtime swapchain format supported for color swapchain");
    }

    return *swapchainFormatIt;
}

const XrBaseInStructure * RVRVulkanRenderer::GetGraphicsBinding() const {
    return reinterpret_cast<const XrBaseInStructure *>(&m_graphicsBinding);
}

std::vector<XrSwapchainImageBaseHeader *> RVRVulkanRenderer::AllocateSwapchainImageStructs(
        uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo) {
    // Allocate and initialize the buffer of image structs (must be sequential in memory for xrEnumerateSwapchainImages).
    // Return back an array of pointers to each swapchain image struct so the consumer doesn't need to know the type/size.
    // Keep the buffer alive by adding it into the list of buffers.
    m_swapchainImageContexts.emplace_back(GetSwapchainImageType());

    SwapchainImageContext &swapchainImageContext = m_swapchainImageContexts.back();

    std::vector<XrSwapchainImageBaseHeader *> bases = swapchainImageContext.Create(
            m_vkDevice, &m_memAllocator, capacity, swapchainCreateInfo, m_pipelineLayout,
            m_shaderProgram, m_drawBuffer);

    // Map every swapchainImage base pointer to this context
    for (auto &base : bases) {
        m_swapchainImageContextMap[base] = &swapchainImageContext;
    }

    return bases;
}

void RVRVulkanRenderer::RenderView(const XrCompositionLayerProjectionView &layerView,
                const XrSwapchainImageBaseHeader *swapchainImage,
                int64_t /*swapchainFormat*/, const std::vector<Cube> &cubes) {
    CHECK(layerView.subImage.imageArrayIndex == 0);  // Texture arrays not supported.

    auto swapchainContext = m_swapchainImageContextMap[swapchainImage];
    uint32_t imageIndex = swapchainContext->ImageIndex(swapchainImage);

    m_cmdBuffer.Reset();
    m_cmdBuffer.Begin();

    // Ensure depth is in the right layout
    swapchainContext->depthBuffer.
    TransitionLayout(&m_cmdBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

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

    vkCmdBeginRenderPass(m_cmdBuffer.buf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(m_cmdBuffer.buf, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchainContext->pipe.pipe);

    // Bind index and vertex buffers
    vkCmdBindIndexBuffer(m_cmdBuffer.buf, m_drawBuffer.idxBuf, 0, VK_INDEX_TYPE_UINT16);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(m_cmdBuffer.buf, 0, 1, &m_drawBuffer.vtxBuf, &offset);

    // Compute the view-projection transform.
    // Note all matrixes (including OpenXR's) are column-major, right-handed.
    const auto &pose = layerView.pose;
    XrMatrix4x4f proj;
    XrMatrix4x4f_CreateProjectionFov(&proj, GRAPHICS_VULKAN, layerView.fov, 0.05f, 100.0f);
    XrMatrix4x4f toView;
    XrVector3f scale{1.f, 1.f, 1.f};
    XrMatrix4x4f_CreateTranslationRotationScale(&toView, &pose.position, &pose.orientation, &scale);
    XrMatrix4x4f view;
    XrMatrix4x4f_InvertRigidBody(&view, &toView);
    XrMatrix4x4f vp;

    // Leave this in here so that we can do benchmarks later. For now just use fast_matrix_mul
    //XrMatrix4x4f_Multiply(&vp, &proj, &view);
    fast_matrix_mul(vp.m, proj.m, view.m);

    // Render each cube
    for (const Cube &cube : cubes) {
        // Compute the model-view-projection transform and push it.
        XrMatrix4x4f model;
        XrMatrix4x4f_CreateTranslationRotationScale(&model, &cube.Pose.position, &cube.Pose.orientation, &cube.Scale);
        XrMatrix4x4f mvp;

        // Leave this in here so that we can do benchmarks later. For now just use fast_matrix_mul
        //XrMatrix4x4f_Multiply(&mvp, &vp, &model);
        fast_matrix_mul(mvp.m, vp.m, model.m);
        vkCmdPushConstants(m_cmdBuffer.buf, m_pipelineLayout.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvp.m), &mvp.m[0]);

        // Draw the cube.
        vkCmdDrawIndexed(m_cmdBuffer.buf, m_drawBuffer.count.idx, 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(m_cmdBuffer.buf);

    m_cmdBuffer.End();
    m_cmdBuffer.Exec(m_vkQueue);

    // XXX Should double-buffer the command buffers, for now just flush
    m_cmdBuffer.Wait();

#if defined(USE_MIRROR_WINDOW)
    // Cycle the window's swapchain on the last view rendered
    if (swapchainContext == &m_swapchainImageContexts.back()) {
        m_swapchain.Acquire();
        m_swapchain.Present(m_vkQueue);
    }
#endif
}

uint32_t RVRVulkanRenderer::GetSupportedSwapchainSampleCount(const XrViewConfigurationView &) {
    return VK_SAMPLE_COUNT_1_BIT;
}

VkBool32 RVRVulkanRenderer::debugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
            size_t /*location*/,
            int32_t /*messageCode*/, const char *pLayerPrefix, const char *pMessage) {
    std::string flagNames;
    std::string objName;
    Log::Level level = Log::Level::Error;

    if ((flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0u) {
        flagNames += "DEBUG:";
        level = Log::Level::Verbose;
    }
    if ((flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0u) {
        flagNames += "INFO:";
        level = Log::Level::Info;
    }
    if ((flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) != 0u) {
        flagNames += "PERF:";
        level = Log::Level::Warning;
    }
    if ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0u) {
        flagNames += "WARN:";
        level = Log::Level::Warning;
    }
    if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0u) {
        flagNames += "ERROR:";
        level = Log::Level::Error;
    }

#define LIST_OBJECT_TYPES(_) \
    _(UNKNOWN)               \
    _(INSTANCE)              \
    _(PHYSICAL_DEVICE)       \
    _(DEVICE)                \
    _(QUEUE)                 \
    _(SEMAPHORE)             \
    _(COMMAND_BUFFER)        \
    _(FENCE)                 \
    _(DEVICE_MEMORY)         \
    _(BUFFER)                \
    _(IMAGE)                 \
    _(EVENT)                 \
    _(QUERY_POOL)            \
    _(BUFFER_VIEW)           \
    _(IMAGE_VIEW)            \
    _(SHADER_MODULE)         \
    _(PIPELINE_CACHE)        \
    _(PIPELINE_LAYOUT)       \
    _(RENDER_PASS)           \
    _(PIPELINE)              \
    _(DESCRIPTOR_SET_LAYOUT) \
    _(SAMPLER)               \
    _(DESCRIPTOR_POOL)       \
    _(DESCRIPTOR_SET)        \
    _(FRAMEBUFFER)           \
    _(COMMAND_POOL)          \
    _(SURFACE_KHR)           \
    _(SWAPCHAIN_KHR)         \
    _(DISPLAY_KHR)           \
    _(DISPLAY_MODE_KHR)

    switch (objectType) {
        default:
#define MK_OBJECT_TYPE_CASE(name)                  \
    case VK_DEBUG_REPORT_OBJECT_TYPE_##name##_EXT: \
        objName = #name;                           \
        break;
        LIST_OBJECT_TYPES(MK_OBJECT_TYPE_CASE)

#if VK_HEADER_VERSION >= 46
        MK_OBJECT_TYPE_CASE(DESCRIPTOR_UPDATE_TEMPLATE_KHR)
#endif
#if VK_HEADER_VERSION >= 70
        MK_OBJECT_TYPE_CASE(DEBUG_REPORT_CALLBACK_EXT)
#endif
    }

    if ((objectType == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT) &&
        (strcmp(pLayerPrefix, "Loader Message") == 0) &&
        (strncmp(pMessage, "Device Extension:", 17) == 0)) {
        return VK_FALSE;
    }

    Log::Write(level, Fmt("%s (%s 0x%llx) [%s] %s", flagNames.c_str(), objName.c_str(), object,
                          pLayerPrefix, pMessage));
    if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0u) {
        return VK_FALSE;
    }
    if ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0u) {
        return VK_FALSE;
    }
    return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
RVRVulkanRenderer::debugReportThunk(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
                 uint64_t object, size_t location, int32_t messageCode,
                 const char *pLayerPrefix, const char *pMessage, void *pUserData) {
    return static_cast<RVRVulkanRenderer *>(pUserData)->debugReport(flags, objectType, object,
                                                                       location, messageCode,
                                                                       pLayerPrefix, pMessage);
}

XrStructureType RVRVulkanRenderer::GetGraphicsBindingType() const {
    return XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR;
}

XrStructureType
RVRVulkanRenderer::GetSwapchainImageType() const {
    return XR_TYPE_SWAPCHAIN_IMAGE_VULKAN2_KHR;
}

XrResult
RVRVulkanRenderer::CreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR *createInfo,
                        VkInstance *vulkanInstance, VkResult *vulkanResult) {
    PFN_xrCreateVulkanInstanceKHR pfnCreateVulkanInstanceKHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrCreateVulkanInstanceKHR",
                                      reinterpret_cast<PFN_xrVoidFunction *>(&pfnCreateVulkanInstanceKHR)));

    return pfnCreateVulkanInstanceKHR(instance, createInfo, vulkanInstance, vulkanResult);
}

XrResult
RVRVulkanRenderer::CreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR *createInfo,
                      VkDevice *vulkanDevice, VkResult *vulkanResult) {
    PFN_xrCreateVulkanDeviceKHR pfnCreateVulkanDeviceKHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrCreateVulkanDeviceKHR",
                                      reinterpret_cast<PFN_xrVoidFunction *>(&pfnCreateVulkanDeviceKHR)));

    return pfnCreateVulkanDeviceKHR(instance, createInfo, vulkanDevice, vulkanResult);
}

XrResult
RVRVulkanRenderer::GetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR *getInfo,
                            VkPhysicalDevice *vulkanPhysicalDevice) {
    PFN_xrGetVulkanGraphicsDevice2KHR pfnGetVulkanGraphicsDevice2KHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsDevice2KHR",
                                      reinterpret_cast<PFN_xrVoidFunction *>(&pfnGetVulkanGraphicsDevice2KHR)));

    return pfnGetVulkanGraphicsDevice2KHR(instance, getInfo, vulkanPhysicalDevice);
}

XrResult RVRVulkanRenderer::GetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId,
                                                   XrGraphicsRequirementsVulkan2KHR *graphicsRequirements) {
    PFN_xrGetVulkanGraphicsRequirements2KHR pfnGetVulkanGraphicsRequirements2KHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsRequirements2KHR",
                                      reinterpret_cast<PFN_xrVoidFunction *>(&pfnGetVulkanGraphicsRequirements2KHR)));

    return pfnGetVulkanGraphicsRequirements2KHR(instance, systemId, graphicsRequirements);
}
