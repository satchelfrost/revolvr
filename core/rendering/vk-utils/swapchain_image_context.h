/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include "cmd_buffer.h"
#include "pipeline.h"
#include "render_target.h"
#include <rendering/gltf/gltf_model.h>
#include <rendering/point_cloud/point_cloud_res.h>
#include <math/transform.h>

namespace rvr {
class SwapchainImageContext {
private:
    std::shared_ptr<RenderingContext> renderingContext_;
    std::vector <XrSwapchainImageVulkan2KHR> swapchainImages_;

    VkExtent2D swapchainExtent_;
    VkViewport viewport_{};
    VkRect2D scissor_{};

    std::vector <std::unique_ptr<RenderTarget>> renderTargets_;
    std::unique_ptr<CmdBuffer> cmdBuffer_;

public:
    SwapchainImageContext(const std::shared_ptr<RenderingContext>& renderingContext, uint32_t capacity,
                          const XrSwapchainCreateInfo &swapchainCreateInfo);

    // Expects a corresponding call to EndRenderPass()
    void BeginRenderPass(uint32_t imageIndex);
    // Expects BeginRenderPass() to have been called
    void EndRenderPass();

    void Draw(const std::unique_ptr<Pipeline>& pipeline, const std::unique_ptr<DrawBuffer>& drawBuffer,
              const std::vector<glm::mat4> &transforms);
    void DrawGltf(const std::unique_ptr<Pipeline>& pipeline, const std::unique_ptr<GLTFModel>& model,
                  VkDescriptorSet descriptorSet);
    void DrawPointCloud(const std::unique_ptr<Pipeline>& pipeline,
                        const std::unique_ptr<PointCloudResource>& pointCloud);
    XrSwapchainImageBaseHeader* GetFirstImagePointer();
    void InitRenderTargets();
};
}
