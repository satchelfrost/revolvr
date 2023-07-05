/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include "pch.h"
#include "common.h"
#include "geometry.h"
#include "math/xr_linear.h"
#include "vulkan_results.h"
#include <array>

struct MemoryAllocator {
    void Init(VkPhysicalDevice physicalDevice, VkDevice device) {
        m_vkDevice = device;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_memProps);
    }

    static const VkFlags defaultFlags =
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    void Allocate(VkMemoryRequirements const &memReqs, VkDeviceMemory *mem,
                  VkFlags flags = defaultFlags,
                  const void *pNext = nullptr) const {
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

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    VkPhysicalDeviceMemoryProperties m_memProps{};
};

// CmdBuffer - manage VkCommandBuffer state
struct CmdBuffer {
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

    ~CmdBuffer() {
        SetState(CmdBufferState::Undefined);
        if (m_vkDevice != nullptr) {
            if (buf != VK_NULL_HANDLE) {
                vkFreeCommandBuffers(m_vkDevice, pool, 1, &buf);
            }
            if (pool != VK_NULL_HANDLE) {
                vkDestroyCommandPool(m_vkDevice, pool, nullptr);
            }
            if (execFence != VK_NULL_HANDLE) {
                vkDestroyFence(m_vkDevice, execFence, nullptr);
            }
        }
        buf = VK_NULL_HANDLE;
        pool = VK_NULL_HANDLE;
        execFence = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

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

#define CHECK_CBSTATE(s)                                                                                           \
    do                                                                                                             \
        if (state != (s)) {                                                                                        \
            Log::Write(Log::Level::Error,                                                                          \
                       std::string("Expecting state " #s " from ") + __FUNCTION__ + ", in " + StateString(state)); \
            return false;                                                                                          \
        }                                                                                                          \
    while (0)

    bool Init(VkDevice device, uint32_t queueFamilyIndex) {
        CHECK_CBSTATE(CmdBufferState::Undefined);

        m_vkDevice = device;

        // Create a command pool to allocate our command buffer from
        VkCommandPoolCreateInfo cmdPoolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
        CHECK_VKCMD(vkCreateCommandPool(m_vkDevice, &cmdPoolInfo, nullptr, &pool));

        // Create the command buffer from the command pool
        VkCommandBufferAllocateInfo cmd{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        cmd.commandPool = pool;
        cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd.commandBufferCount = 1;
        CHECK_VKCMD(vkAllocateCommandBuffers(m_vkDevice, &cmd, &buf));

        VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        CHECK_VKCMD(vkCreateFence(m_vkDevice, &fenceInfo, nullptr, &execFence));

        SetState(CmdBufferState::Initialized);
        return true;
    }

    bool Begin() {
        CHECK_CBSTATE(CmdBufferState::Initialized);
        VkCommandBufferBeginInfo cmdBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        CHECK_VKCMD(vkBeginCommandBuffer(buf, &cmdBeginInfo));
        SetState(CmdBufferState::Recording);
        return true;
    }

    bool End() {
        CHECK_CBSTATE(CmdBufferState::Recording);
        CHECK_VKCMD(vkEndCommandBuffer(buf));
        SetState(CmdBufferState::Executable);
        return true;
    }

    bool Exec(VkQueue queue) {
        CHECK_CBSTATE(CmdBufferState::Executable);

        VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buf;
        CHECK_VKCMD(vkQueueSubmit(queue, 1, &submitInfo, execFence));

        SetState(CmdBufferState::Executing);
        return true;
    }

    bool Wait() {
        // Waiting on a not-in-flight command buffer is a no-op
        if (state == CmdBufferState::Initialized) {
            return true;
        }

        CHECK_CBSTATE(CmdBufferState::Executing);

        const uint32_t timeoutNs = 1 * 1000 * 1000 * 1000;
        for (int i = 0; i < 5; ++i) {
            auto res = vkWaitForFences(m_vkDevice, 1, &execFence, VK_TRUE, timeoutNs);
            if (res == VK_SUCCESS) {
                // Buffer can be executed multiple times...
                SetState(CmdBufferState::Executable);
                return true;
            }
            Log::Write(Log::Level::Info, "Waiting for CmdBuffer fence timed out, retrying...");
        }

        return false;
    }

    bool Reset() {
        if (state != CmdBufferState::Initialized) {
            CHECK_CBSTATE(CmdBufferState::Executable);

            CHECK_VKCMD(vkResetFences(m_vkDevice, 1, &execFence));
            CHECK_VKCMD(vkResetCommandBuffer(buf, 0));

            SetState(CmdBufferState::Initialized);
        }

        return true;
    }

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};

    void SetState(CmdBufferState newState) { state = newState; }

#undef CHECK_CBSTATE
#undef LIST_CMDBUFFER_STATES
};

// ShaderProgram to hold a pair of vertex & fragment shaders
struct ShaderProgram {
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderInfo{
            {{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO},
             {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}}};

    ShaderProgram() = default;

    ~ShaderProgram() {
        if (m_vkDevice != nullptr) {
            for (auto &si: shaderInfo) {
                if (si.module != VK_NULL_HANDLE) {
                    vkDestroyShaderModule(m_vkDevice, shaderInfo[0].module, nullptr);
                }
                si.module = VK_NULL_HANDLE;
            }
        }
        shaderInfo = {};
        m_vkDevice = nullptr;
    }

    ShaderProgram(const ShaderProgram &) = delete;

    ShaderProgram &operator=(const ShaderProgram &) = delete;

    ShaderProgram(ShaderProgram &&) = delete;

    ShaderProgram &operator=(ShaderProgram &&) = delete;

    void LoadVertexShader(const std::vector<char> &code) { Load(0, code); }

    void LoadFragmentShader(const std::vector<char> &code) { Load(1, code); }

    void Init(VkDevice device) { m_vkDevice = device; }

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};

    void Load(uint32_t index, const std::vector<char> &code) {
        VkShaderModuleCreateInfo modInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};

        auto &si = shaderInfo[index];
        si.pName = "main";
        std::string name;

        switch (index) {
            case 0:
                si.stage = VK_SHADER_STAGE_VERTEX_BIT;
                name = "vertex";
                break;
            case 1:
                si.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                name = "fragment";
                break;
            default:
                THROW(Fmt("Unknown code index %d", index));
        }

        modInfo.codeSize = code.size();
        modInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
        CHECK_MSG((modInfo.codeSize > 0) && modInfo.pCode,
                  Fmt("Invalid %s shader ", name.c_str()));

        CHECK_VKCMD(vkCreateShaderModule(m_vkDevice, &modInfo, nullptr, &si.module));

        Log::Write(Log::Level::Info, Fmt("Loaded %s shader", name.c_str()));
    }
};

// VertexBuffer base class
struct VertexBufferBase {
    VkBuffer idxBuf{VK_NULL_HANDLE};
    VkDeviceMemory idxMem{VK_NULL_HANDLE};
    VkBuffer vtxBuf{VK_NULL_HANDLE};
    VkDeviceMemory vtxMem{VK_NULL_HANDLE};
    VkVertexInputBindingDescription bindDesc{};
    std::vector <VkVertexInputAttributeDescription> attrDesc{};
    struct {
        uint32_t idx;
        uint32_t vtx;
    } count = {0, 0};

    VertexBufferBase() = default;

    ~VertexBufferBase() {
        if (m_vkDevice != nullptr) {
            if (idxBuf != VK_NULL_HANDLE) {
                vkDestroyBuffer(m_vkDevice, idxBuf, nullptr);
            }
            if (idxMem != VK_NULL_HANDLE) {
                vkFreeMemory(m_vkDevice, idxMem, nullptr);
            }
            if (vtxBuf != VK_NULL_HANDLE) {
                vkDestroyBuffer(m_vkDevice, vtxBuf, nullptr);
            }
            if (vtxMem != VK_NULL_HANDLE) {
                vkFreeMemory(m_vkDevice, vtxMem, nullptr);
            }
        }
        idxBuf = VK_NULL_HANDLE;
        idxMem = VK_NULL_HANDLE;
        vtxBuf = VK_NULL_HANDLE;
        vtxMem = VK_NULL_HANDLE;
        bindDesc = {};
        attrDesc.clear();
        count = {0, 0};
        m_vkDevice = nullptr;
    }

    VertexBufferBase(const VertexBufferBase &) = delete;

    VertexBufferBase &operator=(const VertexBufferBase &) = delete;

    VertexBufferBase(VertexBufferBase &&) = delete;

    VertexBufferBase &operator=(VertexBufferBase &&) = delete;

    void Init(VkDevice device, const MemoryAllocator *memAllocator,
              const std::vector <VkVertexInputAttributeDescription> &attr) {
        m_vkDevice = device;
        m_memAllocator = memAllocator;
        attrDesc = attr;
    }

protected:
    VkDevice m_vkDevice{VK_NULL_HANDLE};

    void AllocateBufferMemory(VkBuffer buf, VkDeviceMemory *mem) const {
        VkMemoryRequirements memReq = {};
        vkGetBufferMemoryRequirements(m_vkDevice, buf, &memReq);
        m_memAllocator->Allocate(memReq, mem);
    }

private:
    const MemoryAllocator *m_memAllocator{nullptr};
};

// VertexBuffer template to wrap the indices and vertices
template<typename T>
struct VertexBuffer : public VertexBufferBase {
    bool Create(uint32_t idxCount, uint32_t vtxCount) {
        VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufInfo.size = sizeof(uint16_t) * idxCount;
        CHECK_VKCMD(vkCreateBuffer(m_vkDevice, &bufInfo, nullptr, &idxBuf));
        AllocateBufferMemory(idxBuf, &idxMem);
        CHECK_VKCMD(vkBindBufferMemory(m_vkDevice, idxBuf, idxMem, 0));

        bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufInfo.size = sizeof(T) * vtxCount;
        CHECK_VKCMD(vkCreateBuffer(m_vkDevice, &bufInfo, nullptr, &vtxBuf));
        AllocateBufferMemory(vtxBuf, &vtxMem);
        CHECK_VKCMD(vkBindBufferMemory(m_vkDevice, vtxBuf, vtxMem, 0));

        bindDesc.binding = 0;
        bindDesc.stride = sizeof(T);
        bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        count = {idxCount, vtxCount};

        return true;
    }

    void UpdateIndices(const uint16_t *data, uint32_t elements, uint32_t offset = 0) {
        uint16_t *map = nullptr;
        CHECK_VKCMD(vkMapMemory(m_vkDevice, idxMem, sizeof(map[0]) * offset,
                                sizeof(map[0]) * elements, 0, (void **) &map));
        for (size_t i = 0; i < elements; ++i) {
            map[i] = data[i];
        }
        vkUnmapMemory(m_vkDevice, idxMem);
    }

    void UpdateVertices(const T *data, uint32_t elements, uint32_t offset = 0) {
        T *map = nullptr;
        CHECK_VKCMD(vkMapMemory(m_vkDevice, vtxMem, sizeof(map[0]) * offset,
                                sizeof(map[0]) * elements, 0, (void **) &map));
        for (size_t i = 0; i < elements; ++i) {
            map[i] = data[i];
        }
        vkUnmapMemory(m_vkDevice, vtxMem);
    }
};

// RenderPass wrapper
struct RenderPass {
    VkFormat colorFmt{};
    VkFormat depthFmt{};
    VkRenderPass pass{VK_NULL_HANDLE};

    RenderPass() = default;

    bool Create(VkDevice device, VkFormat aColorFmt, VkFormat aDepthFmt) {
        m_vkDevice = device;
        colorFmt = aColorFmt;
        depthFmt = aDepthFmt;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentReference colorRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        VkAttachmentReference depthRef = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        std::array<VkAttachmentDescription, 2> at = {};

        VkRenderPassCreateInfo rpInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
        rpInfo.attachmentCount = 0;
        rpInfo.pAttachments = at.data();
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpass;

        if (colorFmt != VK_FORMAT_UNDEFINED) {
            colorRef.attachment = rpInfo.attachmentCount++;

            at[colorRef.attachment].format = colorFmt;
            at[colorRef.attachment].samples = VK_SAMPLE_COUNT_1_BIT;
            at[colorRef.attachment].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            at[colorRef.attachment].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            at[colorRef.attachment].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            at[colorRef.attachment].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            at[colorRef.attachment].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            at[colorRef.attachment].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorRef;
        }

        if (depthFmt != VK_FORMAT_UNDEFINED) {
            depthRef.attachment = rpInfo.attachmentCount++;

            at[depthRef.attachment].format = depthFmt;
            at[depthRef.attachment].samples = VK_SAMPLE_COUNT_1_BIT;
            at[depthRef.attachment].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            at[depthRef.attachment].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            at[depthRef.attachment].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            at[depthRef.attachment].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            at[depthRef.attachment].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            at[depthRef.attachment].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            subpass.pDepthStencilAttachment = &depthRef;
        }

        CHECK_VKCMD(vkCreateRenderPass(m_vkDevice, &rpInfo, nullptr, &pass));

        return true;
    }

    ~RenderPass() {
        if (m_vkDevice != nullptr) {
            if (pass != VK_NULL_HANDLE) {
                vkDestroyRenderPass(m_vkDevice, pass, nullptr);
            }
        }
        pass = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

    RenderPass(const RenderPass &) = delete;

    RenderPass &operator=(const RenderPass &) = delete;

    RenderPass(RenderPass &&) = delete;

    RenderPass &operator=(RenderPass &&) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};

// VkImage + framebuffer wrapper
struct RenderTarget {
    VkImage colorImage{VK_NULL_HANDLE};
    VkImage depthImage{VK_NULL_HANDLE};
    VkImageView colorView{VK_NULL_HANDLE};
    VkImageView depthView{VK_NULL_HANDLE};
    VkFramebuffer fb{VK_NULL_HANDLE};

    RenderTarget() = default;

    ~RenderTarget() {
        if (m_vkDevice != nullptr) {
            if (fb != VK_NULL_HANDLE) {
                vkDestroyFramebuffer(m_vkDevice, fb, nullptr);
            }
            if (colorView != VK_NULL_HANDLE) {
                vkDestroyImageView(m_vkDevice, colorView, nullptr);
            }
            if (depthView != VK_NULL_HANDLE) {
                vkDestroyImageView(m_vkDevice, depthView, nullptr);
            }
        }

        // Note we don't own color/depthImage, it will get destroyed when xrDestroySwapchain is called
        colorImage = VK_NULL_HANDLE;
        depthImage = VK_NULL_HANDLE;
        colorView = VK_NULL_HANDLE;
        depthView = VK_NULL_HANDLE;
        fb = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

    RenderTarget(RenderTarget &&other)

    noexcept: RenderTarget() {
            using std::swap;
            swap(colorImage, other.colorImage);
            swap(depthImage, other.depthImage);
            swap(colorView, other.colorView);
            swap(depthView, other.depthView);
            swap(fb, other.fb);
            swap(m_vkDevice, other.m_vkDevice);
    }

    RenderTarget &operator=(RenderTarget &&other)

    noexcept {
        if (&other == this) {
            return *this;
        }
        // Clean up ourselves.
        this->~RenderTarget();
        using std::swap;
        swap(colorImage, other.colorImage);
        swap(depthImage, other.depthImage);
        swap(colorView, other.colorView);
        swap(depthView, other.depthView);
        swap(fb, other.fb);
        swap(m_vkDevice, other.m_vkDevice);
        return *this;
    }

    void Create(VkDevice device, VkImage aColorImage, VkImage aDepthImage, VkExtent2D size,
                RenderPass &renderPass) {
        m_vkDevice = device;

        colorImage = aColorImage;
        depthImage = aDepthImage;

        std::array<VkImageView, 2> attachments{};
        uint32_t attachmentCount = 0;

        // Create color image view
        if (colorImage != VK_NULL_HANDLE) {
            VkImageViewCreateInfo colorViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            colorViewInfo.image = colorImage;
            colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            colorViewInfo.format = renderPass.colorFmt;
            colorViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            colorViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            colorViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            colorViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorViewInfo.subresourceRange.baseMipLevel = 0;
            colorViewInfo.subresourceRange.levelCount = 1;
            colorViewInfo.subresourceRange.baseArrayLayer = 0;
            colorViewInfo.subresourceRange.layerCount = 1;
            CHECK_VKCMD(vkCreateImageView(m_vkDevice, &colorViewInfo, nullptr, &colorView));
            attachments[attachmentCount++] = colorView;
        }

        // Create depth image view
        if (depthImage != VK_NULL_HANDLE) {
            VkImageViewCreateInfo depthViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            depthViewInfo.image = depthImage;
            depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            depthViewInfo.format = renderPass.depthFmt;
            depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthViewInfo.subresourceRange.baseMipLevel = 0;
            depthViewInfo.subresourceRange.levelCount = 1;
            depthViewInfo.subresourceRange.baseArrayLayer = 0;
            depthViewInfo.subresourceRange.layerCount = 1;
            CHECK_VKCMD(vkCreateImageView(m_vkDevice, &depthViewInfo, nullptr, &depthView));
            attachments[attachmentCount++] = depthView;
        }

        VkFramebufferCreateInfo fbInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        fbInfo.renderPass = renderPass.pass;
        fbInfo.attachmentCount = attachmentCount;
        fbInfo.pAttachments = attachments.data();
        fbInfo.width = size.width;
        fbInfo.height = size.height;
        fbInfo.layers = 1;
        CHECK_VKCMD(vkCreateFramebuffer(m_vkDevice, &fbInfo, nullptr, &fb));
    }

    RenderTarget(const RenderTarget &) = delete;

    RenderTarget &operator=(const RenderTarget &) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};

// Simple vertex MVP xform & color fragment shader layout
struct PipelineLayout {
    VkPipelineLayout layout{VK_NULL_HANDLE};

    PipelineLayout() = default;

    ~PipelineLayout() {
        if (m_vkDevice != nullptr) {
            if (layout != VK_NULL_HANDLE) {
                vkDestroyPipelineLayout(m_vkDevice, layout, nullptr);
            }
        }
        layout = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

    void Create(VkDevice device) {
        m_vkDevice = device;

        // MVP matrix is a push_constant
        VkPushConstantRange pcr = {};
        pcr.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pcr.offset = 0;
        pcr.size = 4 * 4 * sizeof(float);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
                VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pcr;
        CHECK_VKCMD(vkCreatePipelineLayout(m_vkDevice, &pipelineLayoutCreateInfo, nullptr,
                                           &layout));
    }

    PipelineLayout(const PipelineLayout &) = delete;

    PipelineLayout &operator=(const PipelineLayout &) = delete;

    PipelineLayout(PipelineLayout &&) = delete;

    PipelineLayout &operator=(PipelineLayout &&) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};

// Pipeline wrapper for rendering pipeline state
struct Pipeline {
    VkPipeline pipe{VK_NULL_HANDLE};
    VkPrimitiveTopology topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    std::vector <VkDynamicState> dynamicStateEnables;

    Pipeline() = default;

    void Dynamic(VkDynamicState state) { dynamicStateEnables.emplace_back(state); }

    void
    Create(VkDevice device, VkExtent2D size, const PipelineLayout &layout, const RenderPass &rp,
           const ShaderProgram &sp,
           const VertexBufferBase &vb) {
        m_vkDevice = device;

        VkPipelineDynamicStateCreateInfo dynamicState{
                VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        dynamicState.dynamicStateCount = (uint32_t) dynamicStateEnables.size();
        dynamicState.pDynamicStates = dynamicStateEnables.data();

        VkPipelineVertexInputStateCreateInfo vi{
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        vi.vertexBindingDescriptionCount = 1;
        vi.pVertexBindingDescriptions = &vb.bindDesc;
        vi.vertexAttributeDescriptionCount = (uint32_t) vb.attrDesc.size();
        vi.pVertexAttributeDescriptions = vb.attrDesc.data();

        VkPipelineInputAssemblyStateCreateInfo ia{
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        ia.primitiveRestartEnable = VK_FALSE;
        ia.topology = topology;

        VkPipelineRasterizationStateCreateInfo rs{
                VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        rs.polygonMode = VK_POLYGON_MODE_FILL;
        rs.cullMode = VK_CULL_MODE_BACK_BIT;
        rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rs.depthClampEnable = VK_FALSE;
        rs.rasterizerDiscardEnable = VK_FALSE;
        rs.depthBiasEnable = VK_FALSE;
        rs.depthBiasConstantFactor = 0;
        rs.depthBiasClamp = 0;
        rs.depthBiasSlopeFactor = 0;
        rs.lineWidth = 1.0f;

        VkPipelineColorBlendAttachmentState attachState{};
        attachState.blendEnable = 0;
        attachState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        attachState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachState.colorBlendOp = VK_BLEND_OP_ADD;
        attachState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachState.alphaBlendOp = VK_BLEND_OP_ADD;
        attachState.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo cb{
                VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        cb.attachmentCount = 1;
        cb.pAttachments = &attachState;
        cb.logicOpEnable = VK_FALSE;
        cb.logicOp = VK_LOGIC_OP_NO_OP;
        cb.blendConstants[0] = 1.0f;
        cb.blendConstants[1] = 1.0f;
        cb.blendConstants[2] = 1.0f;
        cb.blendConstants[3] = 1.0f;

        VkRect2D scissor = {{0, 0}, size};
#if defined(ORIGIN_BOTTOM_LEFT)
        // Flipped view so origin is bottom-left like GL (requires VK_KHR_maintenance1)
        VkViewport viewport = {0.0f, (float)size.height, (float)size.width, -(float)size.height, 0.0f, 1.0f};
#else
        // Will invert y after projection
        VkViewport viewport = {0.0f, 0.0f, (float) size.width, (float) size.height, 0.0f, 1.0f};
#endif
        VkPipelineViewportStateCreateInfo vp{
                VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        vp.viewportCount = 1;
        vp.pViewports = &viewport;
        vp.scissorCount = 1;
        vp.pScissors = &scissor;

        VkPipelineDepthStencilStateCreateInfo ds{
                VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
        ds.depthTestEnable = VK_TRUE;
        ds.depthWriteEnable = VK_TRUE;
        ds.depthCompareOp = VK_COMPARE_OP_LESS;
        ds.depthBoundsTestEnable = VK_FALSE;
        ds.stencilTestEnable = VK_FALSE;
        ds.front.failOp = VK_STENCIL_OP_KEEP;
        ds.front.passOp = VK_STENCIL_OP_KEEP;
        ds.front.depthFailOp = VK_STENCIL_OP_KEEP;
        ds.front.compareOp = VK_COMPARE_OP_ALWAYS;
        ds.back = ds.front;
        ds.minDepthBounds = 0.0f;
        ds.maxDepthBounds = 1.0f;

        VkPipelineMultisampleStateCreateInfo ms{
                VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkGraphicsPipelineCreateInfo pipeInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipeInfo.stageCount = (uint32_t) sp.shaderInfo.size();
        pipeInfo.pStages = sp.shaderInfo.data();
        pipeInfo.pVertexInputState = &vi;
        pipeInfo.pInputAssemblyState = &ia;
        pipeInfo.pTessellationState = nullptr;
        pipeInfo.pViewportState = &vp;
        pipeInfo.pRasterizationState = &rs;
        pipeInfo.pMultisampleState = &ms;
        pipeInfo.pDepthStencilState = &ds;
        pipeInfo.pColorBlendState = &cb;
        if (dynamicState.dynamicStateCount > 0) {
            pipeInfo.pDynamicState = &dynamicState;
        }
        pipeInfo.layout = layout.layout;
        pipeInfo.renderPass = rp.pass;
        pipeInfo.subpass = 0;
        CHECK_VKCMD(vkCreateGraphicsPipelines(m_vkDevice, VK_NULL_HANDLE, 1, &pipeInfo, nullptr,
                                              &pipe));
    }

    void Release() {
        if (m_vkDevice != nullptr) {
            if (pipe != VK_NULL_HANDLE) {
                vkDestroyPipeline(m_vkDevice, pipe, nullptr);
            }
        }
        pipe = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};

struct DepthBuffer {
    VkDeviceMemory depthMemory{VK_NULL_HANDLE};
    VkImage depthImage{VK_NULL_HANDLE};

    DepthBuffer() = default;

    ~DepthBuffer() {
        if (m_vkDevice != nullptr) {
            if (depthImage != VK_NULL_HANDLE) {
                vkDestroyImage(m_vkDevice, depthImage, nullptr);
            }
            if (depthMemory != VK_NULL_HANDLE) {
                vkFreeMemory(m_vkDevice, depthMemory, nullptr);
            }
        }
        depthImage = VK_NULL_HANDLE;
        depthMemory = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

    DepthBuffer(DepthBuffer &&other)

    noexcept: DepthBuffer() {
            using std::swap;

            swap(depthImage, other.depthImage);
            swap(depthMemory, other.depthMemory);
            swap(m_vkDevice, other.m_vkDevice);
    }

    DepthBuffer &operator=(DepthBuffer &&other)

    noexcept {
        if (&other == this) {
            return *this;
        }
        // clean up self
        this->~DepthBuffer();
        using std::swap;

        swap(depthImage, other.depthImage);
        swap(depthMemory, other.depthMemory);
        swap(m_vkDevice, other.m_vkDevice);
        return *this;
    }

    void Create(VkDevice device, MemoryAllocator *memAllocator, VkFormat depthFormat,
                const XrSwapchainCreateInfo &swapchainCreateInfo) {
        m_vkDevice = device;

        VkExtent2D size = {swapchainCreateInfo.width, swapchainCreateInfo.height};

        // Create a D32 depthbuffer
        VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.width;
        imageInfo.extent.height = size.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = (VkSampleCountFlagBits) swapchainCreateInfo.sampleCount;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        CHECK_VKCMD(vkCreateImage(device, &imageInfo, nullptr, &depthImage));

        VkMemoryRequirements memRequirements{};
        vkGetImageMemoryRequirements(device, depthImage, &memRequirements);
        memAllocator->Allocate(memRequirements, &depthMemory,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        CHECK_VKCMD(vkBindImageMemory(device, depthImage, depthMemory, 0));
    }

    void TransitionLayout(CmdBuffer *cmdBuffer, VkImageLayout newLayout) {
        if (newLayout == m_vkLayout) {
            return;
        }

        VkImageMemoryBarrier depthBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
        depthBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        depthBarrier.oldLayout = m_vkLayout;
        depthBarrier.newLayout = newLayout;
        depthBarrier.image = depthImage;
        depthBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
        vkCmdPipelineBarrier(cmdBuffer->buf, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                             VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr,
                             0, nullptr, 1, &depthBarrier);

        m_vkLayout = newLayout;
    }

    DepthBuffer(const DepthBuffer &) = delete;

    DepthBuffer &operator=(const DepthBuffer &) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    VkImageLayout m_vkLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};

struct SwapchainImageContext {
    SwapchainImageContext(XrStructureType _swapchainImageType) : swapchainImageType(
            _swapchainImageType) {}

    // A packed array of XrSwapchainImageVulkan2KHR's for xrEnumerateSwapchainImages
    std::vector <XrSwapchainImageVulkan2KHR> swapchainImages;
    std::vector <RenderTarget> renderTarget;
    VkExtent2D size{};
    DepthBuffer depthBuffer{};
    RenderPass rp{};
    Pipeline pipe{};
    XrStructureType swapchainImageType;

    SwapchainImageContext() = default;

    std::vector<XrSwapchainImageBaseHeader *>
    Create(VkDevice device, MemoryAllocator *memAllocator, uint32_t capacity,
           const XrSwapchainCreateInfo &swapchainCreateInfo, const PipelineLayout &layout,
           const ShaderProgram &sp, const VertexBuffer<Geometry::Vertex> &vb) {
        m_vkDevice = device;

        size = {swapchainCreateInfo.width, swapchainCreateInfo.height};
        VkFormat colorFormat = (VkFormat) swapchainCreateInfo.format;
        VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
        // XXX handle swapchainCreateInfo.sampleCount

        depthBuffer.Create(m_vkDevice, memAllocator, depthFormat, swapchainCreateInfo);
        rp.Create(m_vkDevice, colorFormat, depthFormat);
        pipe.Create(m_vkDevice, size, layout, rp, sp, vb);

        swapchainImages.resize(capacity);
        renderTarget.resize(capacity);
        std::vector < XrSwapchainImageBaseHeader * > bases(capacity);
        for (uint32_t i = 0; i < capacity; ++i) {
            swapchainImages[i] = {swapchainImageType};
            bases[i] = reinterpret_cast<XrSwapchainImageBaseHeader *>(&swapchainImages[i]);
        }

        return bases;
    }

    uint32_t ImageIndex(const XrSwapchainImageBaseHeader *swapchainImageHeader) {
        auto p = reinterpret_cast<const XrSwapchainImageVulkan2KHR *>(swapchainImageHeader);
        return (uint32_t)(p - &swapchainImages[0]);
    }

    void BindRenderTarget(uint32_t index, VkRenderPassBeginInfo *renderPassBeginInfo) {
        if (renderTarget[index].fb == VK_NULL_HANDLE) {
            renderTarget[index].Create(m_vkDevice, swapchainImages[index].image,
                                       depthBuffer.depthImage, size, rp);
        }
        renderPassBeginInfo->renderPass = rp.pass;
        renderPassBeginInfo->framebuffer = renderTarget[index].fb;
        renderPassBeginInfo->renderArea.offset = {0, 0};
        renderPassBeginInfo->renderArea.extent = size;
    }

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};


