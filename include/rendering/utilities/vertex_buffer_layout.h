#pragma once

#include <pch.h>
#include <rendering/utilities/vulkan_utils.h>

namespace rvr {
struct VertexAttribute {
    uint32_t bindingIndex;
    DataType type;
    size_t count;
};

class VertexBufferLayout {
private:
    std::vector<VertexAttribute> elements_{};

public:
    void Push(VertexAttribute attribute);
    size_t GetStride() const;
    const std::vector<VertexAttribute> &GetElements() const;
    std::vector<VkVertexInputAttributeDescription> GetVtxAttrDescriptions();
    VkVertexInputBindingDescription GetVertexInputBindingDesc() const;

};
}