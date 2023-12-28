/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

namespace rvr {
class VulkanSampler {
private:
    VkSampler sampler_;
    VkDevice device_;

public:
    VulkanSampler(VkDevice device, VkFilter filter);
    VkSampler GetSampler();
    ~VulkanSampler();
};
}
