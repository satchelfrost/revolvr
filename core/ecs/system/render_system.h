/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <ecs/component/types/point_cloud.h>
#include "ecs/entity/entity.h"
#include "ecs/component/types/spatial.h"
#include "rendering/gltf/gltf_model.h"
#include "rendering/point_cloud/point_cloud_res.h"

namespace rvr::sys::render{
void AppendCubeTransformBuffer(std::vector<math::Transform>& buffer);
void AppendGltfModelPushConstants(std::map<std::string, std::unique_ptr<GLTFModel>>& models);
void AppendPointCloudPushConstants(std::map<std::string, std::unique_ptr<PointCloudResource>>& pointClouds,
                                   glm::mat4 viewProjection);
std::set<std::string> GetUniqueModelNames();
std::set<std::pair<std::string, PointCloud::FileType>> GetUniquePointClouds();
void DrawCubeGrid(std::vector<math::Transform>& buffer);
}
