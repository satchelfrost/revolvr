/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include "ecs/entity/entity.h"
#include "ecs/component/types/spatial.h"
#include "rendering/utilities/gltf/vulkan_gltf_model.h"
#include "rendering/utilities/point_cloud/point_cloud_res.h"

namespace rvr::system::render{
void AppendCubeTransformBuffer(std::vector<math::Transform>& buffer);
void AppendGltfModelPushConstants(std::map<std::string, std::unique_ptr<VulkanGLTFModel>>& models);
void AppendPointCloudPushConstants(std::map<std::string, std::unique_ptr<PointCloudResource>>& pointClouds,
                                   glm::mat4 viewProjection);
std::set<std::string> GetUniqueModelNames();
std::set<std::string> GetUniquePointCloudNames();
void DrawCubeGrid(std::vector<math::Transform>& buffer);
}
