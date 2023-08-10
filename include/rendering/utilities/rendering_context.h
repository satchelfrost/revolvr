#pragma once

#include <pch.h>
#include <rendering/utilities/render_pass.h>

namespace rvr {
class RenderingContext{
private:
    VkPhysicalDevice physicalDevice_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};
    VkQueue graphicsQueue_{VK_NULL_HANDLE};
    VkFormat colorFormat_ = VK_FORMAT_UNDEFINED;
    VkFormat depthFormat_ = VK_FORMAT_D32_SFLOAT;
    RenderPass renderPass_;

    void AllocateBufferMemory(VkBuffer buffer, VkDeviceMemory* memory);

public:
    RenderingContext(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue, VkFormat colorFormat);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset,
                    VkDeviceSize dstOffset);
    uint32_t GetGraphicsQueueFamilyIndex();
    VkDevice GetDevice();
    VkPhysicalDevice GetPhysicalDevice();
    VkQueue GetGraphicsQueue();
    VkFormat GetColorFormat();
    VkFormat GetDepthFormat();
    VkRenderPass GetRenderPass() const;
};
}