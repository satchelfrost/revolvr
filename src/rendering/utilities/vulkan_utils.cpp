#include <rendering/utilities/vulkan_utils.h>
#include <global_context.h>
#include <rendering/utilities/list_vulkan_obj_types.h>
#include <pch.h>

namespace rvr {
VkFormat GetVkFormat(DataType type, uint32_t count) {
    switch (type) {
        case DataType::U8:
            switch (count) {
                case 1: return VK_FORMAT_R8_UNORM;
                case 2: return VK_FORMAT_R8G8_UNORM;
                case 3: return VK_FORMAT_R8G8B8_UNORM;
                case 4: return VK_FORMAT_R8G8B8A8_UNORM;
                default: THROW("Unsupported count");
            }
        case DataType::U16:
            switch (count) {
                case 1:return VK_FORMAT_R16_UINT;
                case 2:return VK_FORMAT_R16G16_UINT;
                case 3: return VK_FORMAT_R16G16B16_UINT;
                case 4: return VK_FORMAT_R16G16B16A16_UINT;
                default: THROW("unsupported count");
            }
        case DataType::U32:
            switch (count) {
                case 1:return VK_FORMAT_R32_UINT;
                case 2:return VK_FORMAT_R32G32_UINT;
                case 3: return VK_FORMAT_R32G32B32_UINT;
                case 4: return VK_FORMAT_R32G32B32A32_UINT;
                default: THROW("unsupported count");
            }
        case DataType::F32:
            switch (count) {
                case 1:return VK_FORMAT_R32_SFLOAT;
                case 2:return VK_FORMAT_R32G32_SFLOAT;
                case 3: return VK_FORMAT_R32G32B32_SFLOAT;
                case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
                default: THROW("unsupported count");
            }
        default: THROW("unsupported enum");
    }
}

size_t DataTypeSize(DataType type) {
    switch(type) {
        case DataType::U8:  return 1;
        case DataType::U16: return 2;
        case DataType::U32:
        case DataType::F32: return 4;
        default: THROW("Unsupported data type");
    }
}

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

// TODO: Move this to rendering context
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

void CheckVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId) {
    XrGraphicsRequirementsVulkan2KHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR};
    CHECK_XRCMD(GetVulkanGraphicsRequirements2KHR(instance, systemId, &graphicsRequirements));
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
    bool found = false;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        if (indices.isComplete()) {
            found = true;
            break;
        }

        i++;
    }
    if (found)
        return indices;
    else
        THROW("Now suitable graphics queue family");
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

void PopulateTransitionLayoutInfo(VkImageMemoryBarrier &barrier, VkPipelineStageFlags& srcStage,
                                  VkPipelineStageFlags& dstStage, VkImageLayout oldLayout, VkImageLayout newLayout) {
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
        switch (newLayout) {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                break;
            default:
                THROW("Unsupported layout transition");
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        THROW("Unsupported layout transition");
    }
}
}