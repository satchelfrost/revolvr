/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include "ecs/entity/entity.h"
#include "ecs/component/types/spatial.h"
#include "rendering/utilities/gltf/vulkan_gltf_model.h"

namespace rvr::system::render{
struct NamedTransform {
    std::string resourceName;
    math::Transform transform;
};
void AppendCubeTransformBuffer(std::vector<math::Transform>& buffer);
void AppendGltfModelPushConstants(std::map<std::string, std::unique_ptr<VulkanGLTFModel>>& models);
std::set<std::string> GetUniqueModelNames();
void DrawCubeGrid(std::vector<math::Transform>& buffer);
}
