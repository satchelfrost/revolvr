#pragma once

#include <pch.h>
#include <rendering/utilities/gltf/vulkan_gltf_model.h>

namespace rvr {
void loadGLTFFile(const std::string fileName, VulkanGLTFModel& gltfModel);
}
