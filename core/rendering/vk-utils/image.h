/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include "rendering_context.h"

namespace rvr {
class Image {
public:
    enum ImageType {
        Depth,
        Color,
        Sample
    };

private:
    VkImage image_;
    VkDeviceMemory imageMemory_;
    VkDevice device_;

public:
    Image(const std::shared_ptr<RenderingContext>& context, ImageType imageType, VkExtent2D extent);
    VkImage GetImage();
    ~Image();
};
}
