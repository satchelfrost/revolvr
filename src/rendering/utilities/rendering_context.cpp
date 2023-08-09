#include <rendering/utilities/rendering_context.h>

#include <rendering/utilities/vulkan_utils.h>
#include <rendering/utilities/rendering_context.h>


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

VkPhysicalDevice RenderingContext::GetPhysicalDevice() {
    return physicalDevice_;
}

VkQueue RenderingContext::GetGraphicsQueue() {
    return graphicsQueue_;
}

VkFormat RenderingContext::GetColorFormat() {
    return colorFormat_;
}
VkFormat RenderingContext::GetDepthFormat() {
    return depthFormat_;
}

VkRenderPass RenderingContext::GetRenderPass() {
    return renderPass_.pass;
}
}
