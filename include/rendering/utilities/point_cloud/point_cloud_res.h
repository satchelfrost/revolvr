/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>
#include <rendering/utilities/vulkan_buffer.h>
#include <glm/glm.hpp>
#include "rendering/utilities/geometry.h"

namespace rvr {
class PointCloudResource {
    std::unique_ptr<VulkanBuffer> vertexBuffer_ = nullptr;
    std::vector<glm::mat4> pushConstantTransforms_;
    std::shared_ptr<RenderingContext> renderingContext_;
    static std::vector<Geometry::Vertex> GetVertexDataFromPly(const std::string& fileName);

public:
    PointCloudResource(std::shared_ptr<RenderingContext> renderingContext, const std::string& fileName);
    void AddPushConstant(glm::mat4 transform);
    void ClearPushConstants();
    void Draw(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout);
};
}
