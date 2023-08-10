/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/command_buffer.h>
#include <rendering//utilities/vulkan_results.h>

#define CHECK_CBSTATE(s)                                                                                           \
    do                                                                                                             \
        if (state != (s)) {                                                                                        \
            Log::Write(Log::Level::Error,                                                                          \
                       std::string("Expecting state " #s " from ") + __FUNCTION__ + ", in " + StateString(state)); \
            return false;                                                                                          \
        }                                                                                                          \
    while (0)

namespace rvr {
CmdBuffer::~CmdBuffer() {
    SetState(CmdBufferState::Undefined);
    if (device_ != nullptr) {
        if (buf != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(device_, pool, 1, &buf);
        }
        if (pool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(device_, pool, nullptr);
        }
        if (execFence != VK_NULL_HANDLE) {
            vkDestroyFence(device_, execFence, nullptr);
        }
    }
    buf = VK_NULL_HANDLE;
    pool = VK_NULL_HANDLE;
    execFence = VK_NULL_HANDLE;
    device_ = nullptr;
}

bool CmdBuffer::Init(const std::shared_ptr<RenderingContext>& context) {
    CHECK_CBSTATE(CmdBufferState::Undefined);

    device_ = context->GetDevice();

    // Create a command pool to allocate our command buffer from
    VkCommandPoolCreateInfo cmdPoolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = context->GetGraphicsQueueFamilyIndex();
    CHECK_VKCMD(vkCreateCommandPool(device_, &cmdPoolInfo, nullptr, &pool));

    // Create the command buffer from the command pool
    VkCommandBufferAllocateInfo cmd{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmd.commandPool = pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;
    CHECK_VKCMD(vkAllocateCommandBuffers(device_, &cmd, &buf));

    VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    CHECK_VKCMD(vkCreateFence(device_, &fenceInfo, nullptr, &execFence));

    SetState(CmdBufferState::Initialized);
    return true;
}

bool CmdBuffer::Begin() {
    CHECK_CBSTATE(CmdBufferState::Initialized);
    VkCommandBufferBeginInfo cmdBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    CHECK_VKCMD(vkBeginCommandBuffer(buf, &cmdBeginInfo));
    SetState(CmdBufferState::Recording);
    return true;
}

bool CmdBuffer::End() {
    CHECK_CBSTATE(CmdBufferState::Recording);
    CHECK_VKCMD(vkEndCommandBuffer(buf));
    SetState(CmdBufferState::Executable);
    return true;
}

bool CmdBuffer::Exec(VkQueue queue) {
    CHECK_CBSTATE(CmdBufferState::Executable);

    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buf;
    CHECK_VKCMD(vkQueueSubmit(queue, 1, &submitInfo, execFence));

    SetState(CmdBufferState::Executing);
    return true;
}

bool CmdBuffer::Wait() {
    // Waiting on a not-in-flight command buffer is a no-op
    if (state == CmdBufferState::Initialized) {
        return true;
    }

    CHECK_CBSTATE(CmdBufferState::Executing);

    const uint32_t timeoutNs = 1 * 1000 * 1000 * 1000;
    for (int i = 0; i < 5; ++i) {
        auto res = vkWaitForFences(device_, 1, &execFence, VK_TRUE, timeoutNs);
        if (res == VK_SUCCESS) {
            // Buffer can be executed multiple times...
            SetState(CmdBufferState::Executable);
            return true;
        }
        PrintInfo("Waiting for CmdBuffer fence timed out, retrying...");
    }

    return false;
}

bool CmdBuffer::Reset() {
    if (state != CmdBufferState::Initialized) {
        CHECK_CBSTATE(CmdBufferState::Executable);

        CHECK_VKCMD(vkResetFences(device_, 1, &execFence));
        CHECK_VKCMD(vkResetCommandBuffer(buf, 0));

        SetState(CmdBufferState::Initialized);
    }

    return true;
}
}
#undef CHECK_CBSTATE