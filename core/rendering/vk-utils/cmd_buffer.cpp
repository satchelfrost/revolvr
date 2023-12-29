/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "cmd_buffer.h"
#include "vk_results.h"

namespace rvr {
CmdBuffer::CmdBuffer(VkDevice device, VkCommandPool pool) : device_(device), pool_(pool) {
    VkCommandBufferAllocateInfo cmd{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmd.commandPool = pool_;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;
    CHECK_VKCMD(vkAllocateCommandBuffers(device_, &cmd, &commandBuffer_));

    VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    CHECK_VKCMD(vkCreateFence(device_, &fenceInfo, nullptr, &fence_));
}

CmdBuffer::~CmdBuffer() {
    Wait();
    if (device_ != nullptr) {
        if (commandBuffer_ != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(device_, pool_, 1, &commandBuffer_);
        }
        if (fence_ != VK_NULL_HANDLE) {
            vkDestroyFence(device_, fence_, nullptr);
        }
    }
    commandBuffer_ = VK_NULL_HANDLE;
    fence_ = VK_NULL_HANDLE;
    device_ = nullptr;
}

void CmdBuffer::Begin() {
    VkCommandBufferBeginInfo info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    CHECK_VKCMD(vkBeginCommandBuffer(commandBuffer_, &info));
}

void CmdBuffer::End() {
    CHECK_VKCMD(vkEndCommandBuffer(commandBuffer_));
}

void CmdBuffer::Exec(VkQueue queue) {
    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer_;
    CHECK_VKCMD(vkQueueSubmit(queue, 1, &submitInfo, fence_));
}

void CmdBuffer::Wait() {
    const uint32_t timeoutNs = 1 * 1000 * 1000 * 1000;
    for (int i = 0; i < 5; ++i) {
        VkResult result = vkWaitForFences(device_, 1, &fence_,
                                          VK_TRUE, timeoutNs);
        if (result == VK_SUCCESS)
            return;
        PrintWarning("Waiting for CmdBuffer fence timed out, retrying...");
    }
}

void CmdBuffer::Reset() {
    CHECK_VKCMD(vkResetFences(device_, 1, &fence_));
    CHECK_VKCMD(vkResetCommandBuffer(commandBuffer_, 0));
}

VkCommandBuffer CmdBuffer::GetBuffer() {
    return commandBuffer_;
}
}
