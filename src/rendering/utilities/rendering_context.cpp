#include <rendering/utilities/rendering_context.h>

#include <rendering/utilities/vulkan_utils.h>

namespace rvr {
RenderingContext::RenderingContext(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue,
                                   VkFormat colorFormat) :
physicalDevice_(physicalDevice), device_(device), graphicsQueue_(graphicsQueue), colorFormat_(colorFormat) {
    renderPass_.Create(device, colorFormat, depthFormat_);
}

uint32_t RenderingContext::GetGraphicsQueueFamilyIndex() {
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_);
    indices.graphicsFamily.value();
}

VkDevice RenderingContext::GetDevice() {
    return device_;
}
}
