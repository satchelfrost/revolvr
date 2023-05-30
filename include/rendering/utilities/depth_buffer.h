#pragma once

#include <pch.h>
#include <common.h>

struct DepthBuffer {
    VkDeviceMemory depthMemory{VK_NULL_HANDLE};
    VkImage depthImage{VK_NULL_HANDLE};

    DepthBuffer() = default;

    ~DepthBuffer() {
        if (m_vkDevice != nullptr) {
            if (depthImage != VK_NULL_HANDLE) {
                vkDestroyImage(m_vkDevice, depthImage, nullptr);
            }
            if (depthMemory != VK_NULL_HANDLE) {
                vkFreeMemory(m_vkDevice, depthMemory, nullptr);
            }
        }
        depthImage = VK_NULL_HANDLE;
        depthMemory = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

    DepthBuffer(DepthBuffer &&other) noexcept: DepthBuffer() {
            using std::swap;

            swap(depthImage, other.depthImage);
            swap(depthMemory, other.depthMemory);
            swap(m_vkDevice, other.m_vkDevice);
    }

    DepthBuffer &operator=(DepthBuffer &&other) noexcept {
        if (&other == this) {
            return *this;
        }
        // clean up self
        this->~DepthBuffer();
        using std::swap;

        swap(depthImage, other.depthImage);
        swap(depthMemory, other.depthMemory);
        swap(m_vkDevice, other.m_vkDevice);
        return *this;
    }

    void Create(VkDevice device, MemoryAllocator *memAllocator, VkFormat depthFormat,
                const XrSwapchainCreateInfo &swapchainCreateInfo) {
        m_vkDevice = device;

        VkExtent2D size = {swapchainCreateInfo.width, swapchainCreateInfo.height};

        // Create a D32 depthbuffer
        VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.width;
        imageInfo.extent.height = size.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = (VkSampleCountFlagBits) swapchainCreateInfo.sampleCount;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        CHECK_VKCMD(vkCreateImage(device, &imageInfo, nullptr, &depthImage));

        VkMemoryRequirements memRequirements{};
        vkGetImageMemoryRequirements(device, depthImage, &memRequirements);
        memAllocator->Allocate(memRequirements, &depthMemory,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        CHECK_VKCMD(vkBindImageMemory(device, depthImage, depthMemory, 0));
    }

    void TransitionLayout(CmdBuffer *cmdBuffer, VkImageLayout newLayout) {
        if (newLayout == m_vkLayout) {
            return;
        }

        VkImageMemoryBarrier depthBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
        depthBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        depthBarrier.oldLayout = m_vkLayout;
        depthBarrier.newLayout = newLayout;
        depthBarrier.image = depthImage;
        depthBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
        vkCmdPipelineBarrier(cmdBuffer->buf, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                             VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr,
                             0, nullptr, 1, &depthBarrier);

        m_vkLayout = newLayout;
    }

    DepthBuffer(const DepthBuffer &) = delete;

    DepthBuffer &operator=(const DepthBuffer &) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    VkImageLayout m_vkLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};

