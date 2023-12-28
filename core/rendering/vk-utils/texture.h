/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <ktx.h>
#include "sampler.h"
#include "rendering_context.h"
#include "view.h"
#include "image.h"

namespace rvr {
struct Texture {
    VkDevice device;
    std::shared_ptr<Image> image;
    std::unique_ptr<View> view;
    std::unique_ptr<Sampler> sampler;
    VkImageLayout imageLayout;
    uint32_t width, height;
    uint32_t mipLevels;
    uint32_t layerCount;
    VkDescriptorImageInfo descriptor;

    void UpdateDescriptor();
    void Destroy();
    void FromBuffer(
            void *             buffer,
            VkDeviceSize       bufferSize,
            uint32_t           texWidth,
            uint32_t           texHeight,
            const std::shared_ptr<RenderingContext>& renderingContext,
            VkFilter           filter          = VK_FILTER_LINEAR,
            VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};
}
