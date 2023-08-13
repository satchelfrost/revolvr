#pragma once

#include <pch.h>
#include <rendering/utilities/render_pass.h>

namespace rvr {
class RenderingContext{
private:
    VkPhysicalDevice physicalDevice_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};
    VkQueue graphicsQueue_{VK_NULL_HANDLE};
    VkCommandPool graphicsPool_;
    VkFormat colorFormat_ = VK_FORMAT_UNDEFINED;
    VkFormat depthFormat_ = VK_FORMAT_D32_SFLOAT; // TODO: Find format
    RenderPass renderPass_;

    void AllocateBufferMemory(VkBuffer buffer, VkDeviceMemory* memory);
    void AllocateImageMemory(VkImage image, VkDeviceMemory* memory);
public:
    RenderingContext(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue, VkFormat colorFormat,
                     VkCommandPool graphicsPool);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset,
                    VkDeviceSize dstOffset);
    void CreateImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags usage, VkImage* image,
                     VkDeviceMemory* imageMemory);
    void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectMask, VkImageView *imageView);
    void CreateTransitionLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

//    uint32_t GetGraphicsQueueFamilyIndex();
    VkDevice GetDevice();
    VkPhysicalDevice GetPhysicalDevice();
    VkQueue GetGraphicsQueue();
    VkCommandPool GetGraphicsPool();
    VkFormat GetColorFormat();
    VkFormat GetDepthFormat();
    VkRenderPass GetRenderPass() const;
};
}