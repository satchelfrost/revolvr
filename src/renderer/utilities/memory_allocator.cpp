#include <rendering/utilities/memory_allocator.h>
#include <rendering//utilities/vulkan_results.h>

void MemoryAllocator::Init(VkPhysicalDevice physicalDevice, VkDevice device) {
    m_vkDevice = device;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_memProps);
}

void
MemoryAllocator::Allocate(const VkMemoryRequirements &memReqs, VkDeviceMemory *mem, VkFlags flags,
                          const void *pNext) const {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < m_memProps.memoryTypeCount; ++i) {
        if ((memReqs.memoryTypeBits & (1 << i)) != 0u) {
            // Type is available, does it match user properties?
            if ((m_memProps.memoryTypes[i].propertyFlags & flags) == flags) {
                VkMemoryAllocateInfo memAlloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                              pNext};
                memAlloc.allocationSize = memReqs.size;
                memAlloc.memoryTypeIndex = i;
                CHECK_VKCMD(vkAllocateMemory(m_vkDevice, &memAlloc, nullptr, mem));
                return;
            }
        }
    }
    THROW("Memory format not supported");
}
