#pragma once

#include <pch.h>
#include <common.h>

class MemoryAllocator {
public:
    void Init(VkPhysicalDevice physicalDevice, VkDevice device);

    static const VkFlags defaultFlags =
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    void Allocate(VkMemoryRequirements const &memReqs, VkDeviceMemory *mem,
                  VkFlags flags = defaultFlags,
                  const void *pNext = nullptr) const;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    VkPhysicalDeviceMemoryProperties m_memProps{};
};


