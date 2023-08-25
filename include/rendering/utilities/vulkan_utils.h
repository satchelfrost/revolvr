#pragma once
#include <pch.h>

namespace rvr {
// TODO: move some of these enums and structs to vulkan data types
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() {return graphicsFamily.has_value();}
};

enum class DataType {
    U8,
    U16,
    U32,
    F32
};

enum class MemoryType {
    DeviceLocal,
    HostVisible
};

VkFormat GetVkFormat(DataType type, uint32_t count);
size_t DataTypeSize(DataType type);
VkResult CreateDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackCreateInfoEXT* createInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugReportCallbackEXT* callback);
void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback,
                                   const VkAllocationCallbacks* pAllocator);
VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags,
                                                   VkDebugReportObjectTypeEXT objectType, uint64_t object,
                                                   size_t location, int32_t messageCode,
                                                   const char *pLayerPrefix,
                                                   const char *pMessage, void *pUserData);
std::vector<char> CreateSPIRVVector(const char* assetName);
int64_t SelectColorSwapchainFormat(const std::vector<int64_t>& runtimeFormats);
XrResult CreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo,
                                 VkInstance* vulkanInstance, VkResult* vulkanResult);
XrResult CreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR* createInfo,
                               VkDevice* vulkanDevice, VkResult* vulkanResult);
XrResult GetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo,
                                     VkPhysicalDevice* vulkanPhysicalDevice);
XrResult GetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId,
                                           XrGraphicsRequirementsVulkan2KHR* graphicsRequirements);
void CheckVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId);
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t filter, VkMemoryPropertyFlags flags);
void PopulateTransitionLayoutInfo(VkImageMemoryBarrier& barrier, VkPipelineStageFlags& srcStage,
                                  VkPipelineStageFlags& dstStage, VkImageLayout oldLayout, VkImageLayout newLayout);
VkMemoryPropertyFlags GetMemoryPropertyFlags(MemoryType memType);
}