#include <rendering/utilities/vulkan_utils.h>
#include <global_context.h>
#include <rendering/utilities/list_vulkan_obj_types.h>
#include <pch.h>

namespace rvr {
VkResult CreateDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackCreateInfoEXT* createInfo,
                                      const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* callback) {
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func)
        return func(instance, createInfo, pAllocator, callback);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback,
                                   const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr)
        func(instance, callback, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags,
                                                   VkDebugReportObjectTypeEXT objectType, uint64_t object,
                                                   size_t location, int32_t messageCode,
                                                   const char *pLayerPrefix,
                                                   const char *pMessage, void *pUserData) {
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

    switch (objectType) {
        default:
        #define MK_OBJECT_TYPE_CASE(name) case VK_DEBUG_REPORT_OBJECT_TYPE_##name##_EXT: objName = #name; break;
        LIST_OBJECT_TYPES(MK_OBJECT_TYPE_CASE)
        #if VK_HEADER_VERSION >= 46
            MK_OBJECT_TYPE_CASE(DESCRIPTOR_UPDATE_TEMPLATE_KHR)
        #endif
        #if VK_HEADER_VERSION >= 70
            MK_OBJECT_TYPE_CASE(DEBUG_REPORT_CALLBACK_EXT)
        #endif
        #undef MK_OBJECT_TYPE_CASE
    }

    if ((objectType == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT) &&
        (strcmp(pLayerPrefix, "Loader Message") == 0) &&
        (strncmp(pMessage, "Device Extension:", 17) == 0)) {
        return VK_FALSE;
    }

    Log::Write(level, Fmt("%s (%s 0x%llx) [%s] %s", flagNames.c_str(), objName.c_str(), object,
                                        pLayerPrefix, pMessage));

    return VK_FALSE;
}

const char* GetValidationLayerName() {
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

std::vector<char> CreateSPIRVVector(const char* assetName) {
    AAsset *file = AAssetManager_open(GlobalContext::Inst()->GetAndroidContext()->GetAndroidAssetManager(),
                                      assetName,
                                      AASSET_MODE_BUFFER);
    off_t fileLength = AAsset_getLength(file);
    char *fileContent = new char[fileLength];
    AAsset_read(file, fileContent, fileLength);
    std::vector<char> shader_vector(fileContent, fileContent + fileLength);
    delete[] fileContent;
    return shader_vector;
}

int64_t SelectColorSwapchainFormat(const std::vector<int64_t> &runtimeFormats) {
    // List of supported color swapchain formats.
    constexpr int64_t SupportedColorSwapchainFormats[] = {VK_FORMAT_B8G8R8A8_SRGB,
                                                          VK_FORMAT_R8G8B8A8_SRGB,
                                                          VK_FORMAT_B8G8R8A8_UNORM,
                                                          VK_FORMAT_R8G8B8A8_UNORM};

    auto swapchainFormatIt = std::find_first_of(runtimeFormats.begin(),
                                                runtimeFormats.end(),
                                                std::begin(SupportedColorSwapchainFormats),
                                                std::end(SupportedColorSwapchainFormats));
    if (swapchainFormatIt == runtimeFormats.end())
        THROW("No runtime swapchain format supported for color swapchain");
    return *swapchainFormatIt;
}
XrResult CreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR *createInfo,
                                 VkInstance *vulkanInstance, VkResult *vulkanResult) {
    PFN_xrCreateVulkanInstanceKHR pfnCreateVulkanInstanceKHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrCreateVulkanInstanceKHR",
                                      reinterpret_cast<PFN_xrVoidFunction *>(&pfnCreateVulkanInstanceKHR)));

    return pfnCreateVulkanInstanceKHR(instance, createInfo, vulkanInstance, vulkanResult);
}

XrResult CreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR *createInfo,
                               VkDevice *vulkanDevice, VkResult *vulkanResult) {
    PFN_xrCreateVulkanDeviceKHR pfnCreateVulkanDeviceKHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrCreateVulkanDeviceKHR",
                                      reinterpret_cast<PFN_xrVoidFunction *>(&pfnCreateVulkanDeviceKHR)));
    return pfnCreateVulkanDeviceKHR(instance, createInfo, vulkanDevice, vulkanResult);
}

XrResult GetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR *getInfo,
                                     VkPhysicalDevice *vulkanPhysicalDevice) {
    PFN_xrGetVulkanGraphicsDevice2KHR pfnGetVulkanGraphicsDevice2KHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsDevice2KHR",
                                      reinterpret_cast<PFN_xrVoidFunction *>(&pfnGetVulkanGraphicsDevice2KHR)));
    return pfnGetVulkanGraphicsDevice2KHR(instance, getInfo, vulkanPhysicalDevice);
}

XrResult GetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId,
                                           XrGraphicsRequirementsVulkan2KHR *graphicsRequirements) {
    PFN_xrGetVulkanGraphicsRequirements2KHR pfnGetVulkanGraphicsRequirements2KHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsRequirements2KHR",
                                      reinterpret_cast<PFN_xrVoidFunction *>(&pfnGetVulkanGraphicsRequirements2KHR)));
    return pfnGetVulkanGraphicsRequirements2KHR(instance, systemId, graphicsRequirements);
}

void CheckVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId,
                                         XrGraphicsRequirementsVulkan2KHR* graphicsRequirements) {
    CHECK_XRCMD(GetVulkanGraphicsRequirements2KHR(instance, systemId, graphicsRequirements));
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice) {
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             queueFamilies.data());

    // Here we want the queue family to support graphics
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        if (indices.isComplete())
            break;

        i++;
    }
    return indices;
}

uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t filter, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);
    for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
        if (filter & (1u << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
            return i;
    THROW("Failed to find suitable memory type");
}

VkMemoryAllocateInfo CreateMemAllocInfo(VkDeviceSize size, uint32_t memIdx) {
    VkMemoryAllocateInfo info{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    info.allocationSize = size;
    info.memoryTypeIndex = memIdx;
    return info;
}
}