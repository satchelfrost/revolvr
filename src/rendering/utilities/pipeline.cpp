/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/pipeline.h>
#include <rendering/utilities/render_pass.h>
#include <rendering/utilities/shader_program.h>
#include <rendering/utilities/draw_buffer.h>
#include <rendering//utilities/vulkan_results.h>

namespace rvr {
PipelineLayout::~PipelineLayout() {
    if (device_ != nullptr) {
        if (layout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(device_, layout, nullptr);
        }
    }
    layout = VK_NULL_HANDLE;
    device_ = nullptr;
}

void PipelineLayout::Create(VkDevice device) {
    device_ = device;

    VkPushConstantRange pcr = {};
    pcr.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pcr.offset = 0;
    pcr.size = 4 * 4 * sizeof(float);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pcr;
    CHECK_VKCMD(vkCreatePipelineLayout(device_, &pipelineLayoutCreateInfo, nullptr,
                                       &layout));
}

void Pipeline::Create(VkDevice device, const RenderPass &renderPass, const ShaderProgram &shaderProgram,
                      const DrawBuffer &drawBuffer) {
    device_ = device;
    pipelineLayout_.Create(device_);

    VkPipelineDynamicStateCreateInfo dynamicState{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamicState.dynamicStateCount = (uint32_t) dynamicStateEnables_.size();
    dynamicState.pDynamicStates = dynamicStateEnables_.data();

    VkPipelineVertexInputStateCreateInfo vi{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vi.vertexBindingDescriptionCount = 1;
    vi.pVertexBindingDescriptions = &drawBuffer.bindDesc;
    vi.vertexAttributeDescriptionCount = (uint32_t) drawBuffer.attrDesc.size();
    vi.pVertexAttributeDescriptions = drawBuffer.attrDesc.data();

    VkPipelineInputAssemblyStateCreateInfo ia{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = topology_;

    VkPipelineRasterizationStateCreateInfo rs{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
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

    VkPipelineColorBlendStateCreateInfo cb{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    cb.attachmentCount = 1;
    cb.pAttachments = &attachState;
    cb.logicOpEnable = VK_FALSE;
    cb.logicOp = VK_LOGIC_OP_NO_OP;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;

    VkPipelineViewportStateCreateInfo vp{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    vp.viewportCount = 1;
    vp.pViewports = VK_NULL_HANDLE;
    vp.scissorCount = 1;
    vp.pScissors = VK_NULL_HANDLE;

    VkPipelineDepthStencilStateCreateInfo ds{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
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

    VkPipelineMultisampleStateCreateInfo ms{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkGraphicsPipelineCreateInfo pipeInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipeInfo.stageCount = (uint32_t) shaderProgram.shaderInfo.size();
    pipeInfo.pStages = shaderProgram.shaderInfo.data();
    pipeInfo.pVertexInputState = &vi;
    pipeInfo.pInputAssemblyState = &ia;
    pipeInfo.pTessellationState = nullptr;
    pipeInfo.pViewportState = &vp;
    pipeInfo.pRasterizationState = &rs;
    pipeInfo.pMultisampleState = &ms;
    pipeInfo.pDepthStencilState = &ds;
    pipeInfo.pColorBlendState = &cb;
    if (dynamicState.dynamicStateCount > 0)
        pipeInfo.pDynamicState = &dynamicState;
    pipeInfo.layout = pipelineLayout_.layout;
    pipeInfo.renderPass = renderPass.pass;
    pipeInfo.subpass = 0;
    CHECK_VKCMD(vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1,
                                          &pipeInfo, nullptr,&pipeline_));
}

void Pipeline::Dynamic(VkDynamicState state) {
    dynamicStateEnables_.emplace_back(state);
}

void Pipeline::Release() {
    if (device_ != nullptr && pipeline_ != VK_NULL_HANDLE)
        vkDestroyPipeline(device_, pipeline_, nullptr);

    pipeline_ = VK_NULL_HANDLE;
    device_ = nullptr;
}
}