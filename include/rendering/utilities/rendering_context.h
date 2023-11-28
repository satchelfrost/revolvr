/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

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
//    VkFormat depthFormat_ = VK_FORMAT_D32_SFLOAT;
//    VkFormat depthFormat_ = VK_FORMAT_D32_SFLOAT_S8_UINT;
    VkFormat depthFormat_ = VK_FORMAT_D24_UNORM_S8_UINT;
//    VkFormat depthFormat_ = VK_FORMAT_D16_UNORM_S8_UINT;
    std::unique_ptr<RenderPass> renderPass_;

    void AllocateBufferMemory(VkBuffer buffer, VkDeviceMemory* memory, VkMemoryPropertyFlags memoryPropertyFlags);
    void AllocateImageMemory(VkImage image, VkDeviceMemory* memory, VkMemoryPropertyFlags memoryPropertyFlags);
    void AllocateMemory(VkDeviceMemory* memory, VkMemoryPropertyFlags memoryPropertyFlags,
                        VkMemoryRequirements memoryRequirements);
public:
    RenderingContext(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue, VkFormat colorFormat,
                     VkCommandPool graphicsPool);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags,
                      VkBuffer* buffer, VkDeviceMemory* bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset,
                    VkDeviceSize dstOffset);
    void CreateImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags usage, VkImage* image,
                     VkDeviceMemory* imageMemory,
                     VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask, VkImageView *imageView);
    void CreateTransitionLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    VkDevice GetDevice();
    VkQueue GetGraphicsQueue();
    VkCommandPool GetGraphicsPool();
    VkFormat GetColorFormat();
    VkFormat GetDepthFormat();
    VkRenderPass GetRenderPass() const;
};
}
