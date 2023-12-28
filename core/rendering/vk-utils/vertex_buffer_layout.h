/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include "vk_utils.h"

namespace rvr {
struct VertexAttribute {
    uint32_t bindingIndex;
    DataType type;
    size_t count;
    std::string debugName;
};

class VertexBufferLayout {
private:
    std::vector<VertexAttribute> elements_{};

public:
    void Push(const VertexAttribute& attribute);
    size_t GetStride() const;
    const std::vector<VertexAttribute> &GetElements() const;
    std::vector<VkVertexInputAttributeDescription> GetVtxAttrDescriptions();
    VkVertexInputBindingDescription GetVertexInputBindingDesc() const;

};
}
