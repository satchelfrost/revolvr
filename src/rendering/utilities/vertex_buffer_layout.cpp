/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/vertex_buffer_layout.h>

namespace rvr {
void VertexBufferLayout::Push(const VertexAttribute& attribute) {
    elements_.push_back(attribute);
}

size_t VertexBufferLayout::GetStride() const {
    size_t size = 0;
    for (const auto& element : elements_)
        size += element.count * DataTypeSize(element.type);
    return size;
}

const std::vector<VertexAttribute> &VertexBufferLayout::GetElements() const {
    return elements_;
}

std::vector<VkVertexInputAttributeDescription> VertexBufferLayout::GetVtxAttrDescriptions() {
   std::vector<VkVertexInputAttributeDescription> attrDescriptions{};
   uint32_t offset = 0;
   for (const auto& element : elements_) {
       VkVertexInputAttributeDescription description{};
       description.location = element.bindingIndex;
       description.binding = 0;
       description.format = GetVkFormat(element.type, element.count);
       description.offset = offset;

       attrDescriptions.push_back(description);
       offset += element.count * DataTypeSize(element.type);
   }
    return attrDescriptions;
}

VkVertexInputBindingDescription VertexBufferLayout::GetVertexInputBindingDesc() const {
    VkVertexInputBindingDescription description{};
    description.binding = 0;
    description.stride = GetStride();
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return description;
}
}