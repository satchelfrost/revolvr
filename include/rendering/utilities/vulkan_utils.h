#pragma once
#include <pch.h>

namespace rvr {
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() {return graphicsFamily.has_value();}
};

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
VkMemoryAllocateInfo CreateMemAllocInfo(VkDeviceSize size, uint32_t memIdx);
void PopulateTransitionLayoutInfo(VkImageMemoryBarrier& barrier, VkPipelineStageFlags& srcStage,
                                  VkPipelineStageFlags& dstStage, VkImageLayout oldLayout, VkImageLayout newLayout);
}