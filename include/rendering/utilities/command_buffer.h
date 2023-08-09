/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/rendering_context.h>

namespace rvr {
class CmdBuffer {
public:

#define LIST_CMDBUFFER_STATES(_) \
    _(Undefined)                 \
    _(Initialized)               \
    _(Recording)                 \
    _(Executable)                \
    _(Executing)

    enum class CmdBufferState {
#define MK_ENUM(name) name,
        LIST_CMDBUFFER_STATES(MK_ENUM)
#undef MK_ENUM
    };

    CmdBufferState state{CmdBufferState::Undefined};
    VkCommandPool pool{VK_NULL_HANDLE};
    VkCommandBuffer buf{VK_NULL_HANDLE};
    VkFence execFence{VK_NULL_HANDLE};

    CmdBuffer() = default;
    CmdBuffer(const CmdBuffer &) = delete;
    CmdBuffer &operator=(const CmdBuffer &) = delete;
    CmdBuffer(CmdBuffer &&) = delete;
    CmdBuffer &operator=(CmdBuffer &&) = delete;
    ~CmdBuffer();

    std::string StateString(CmdBufferState s) {
        switch (s) {
#define MK_CASE(name)          \
    case CmdBufferState::name: \
        return #name;
            LIST_CMDBUFFER_STATES(MK_CASE)
#undef MK_CASE
        }
        return "(Unknown)";
    }

    bool Init(const std::shared_ptr<RenderingContext>& context);
    bool Begin();
    bool End();
    bool Exec(VkQueue queue);
    bool Wait();
    bool Reset();

private:
    VkDevice device_{VK_NULL_HANDLE};

    void SetState(CmdBufferState newState) { state = newState; }

#undef LIST_CMDBUFFER_STATES
};
}