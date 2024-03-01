/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/system/render_system.h>
#include <ecs/system/spatial_system.h>
#include <ecs/component/types/mesh.h>
#include <ecs/component/types/point_cloud.h>
#include <global_context.h>

namespace rvr::sys::render {
void AppendCubeTransformBuffer(std::vector<math::Transform>& buffer) {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Mesh);
    for (auto [eid, component] : components) {
        auto mesh = dynamic_cast<Mesh*>(component);
        if (mesh->IsVisible() && !mesh->HasResource()) {
            auto* spatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(eid);
            buffer.push_back(spatial::GetPlayerRelativeTransform(spatial));
        }
    }
//    DrawCubeGrid(buffer);
}

void AppendGltfModelPushConstants(std::map<std::string, std::unique_ptr<GLTFModel>>& models) {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Mesh);
    for (auto [eid, component] : components) {
        auto mesh = dynamic_cast<Mesh*>(component);
        if (mesh->IsVisible() && mesh->HasResource()) {
            auto* spatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(eid);
            std::string resourceName = mesh->ResourceName();
            math::Transform transform = spatial::GetPlayerRelativeTransform(spatial);
            if (models.count(resourceName))
                models[resourceName]->AddPushConstant(transform.ToMat4());
            else
                rvr::PrintWarning("Resource " + resourceName + " has not been loaded, cannot add push constant");
        }
    }
}

void AppendPointCloudPushConstants(std::map<std::string, std::unique_ptr<PointCloudResource>>& pointClouds,
                                   glm::mat4 viewProjection) {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::PointCloud);
    for (auto [eid, component] : components) {
        auto pointCloud = dynamic_cast<PointCloud*>(component);
        if (pointCloud->IsVisible() && pointCloud->HasResource()) {
            auto* spatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(eid);
            std::string resourceName = pointCloud->FullResourceName();
            math::Transform transform = spatial::GetPlayerRelativeTransform(spatial);
            if (pointClouds.count(resourceName))
                pointClouds[resourceName]->AddPushConstant(viewProjection * transform.ToMat4());
            else
                rvr::PrintWarning("Resource " + resourceName + " has not been loaded, cannot add push constant");
        }
    }
}

std::set<std::string> GetUniqueModelNames() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Mesh);
    std::set<std::string> uniqueNames;
    for (auto [eid, component] : components) {
        auto mesh = dynamic_cast<Mesh*>(component);
        if (mesh->HasResource())
            uniqueNames.insert(mesh->ResourceName());
    }
    return uniqueNames;
}

std::set<std::string> GetUniquePointCloudNames() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::PointCloud);
    std::set<std::string> uniqueNames;
    for (auto [eid, component] : components) {
        auto pointCloud = dynamic_cast<PointCloud*>(component);
        uniqueNames.insert(pointCloud->FullResourceName());
    }
    return uniqueNames;
}

void DrawCubeGrid(std::vector<math::Transform>& buffer) {
    // Get the player and spatial world transforms
    auto player = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(GlobalContext::Inst()->PLAYER_ID);
    CHECK_MSG(player, "Player spatial does not exist inside of GetPlayerRelativeTransform()");
    math::Transform playerWorld = player->GetWorld();
    auto invOrientation = glm::inverse(playerWorld.GetOrientation());

    // Draw the horizontal lines
    for (int i = -5; i <= 5; i ++) {
        math::Transform cube;
        glm::vec3 position(0, 0, (float)i * 2.0f);
        auto relativePosition = position - playerWorld.GetPosition();
        cube.SetPosition(invOrientation * relativePosition);
        cube.SetOrientation(invOrientation);
        cube.SetScale(20.0f, 0.1f, 0.1f);
        buffer.push_back(cube);
    }
    // Draw the vertical lines
    for (int i = -5; i <= 5; i ++) {
        math::Transform cube;
        glm::vec3 position((float)i * 2.0f, 0, 0);
        auto relativePosition = position - playerWorld.GetPosition();
        cube.SetPosition(invOrientation * relativePosition);
        cube.SetOrientation(invOrientation);
        cube.SetScale(0.1f, 0.1f, 20.0f);
        buffer.push_back(cube);
    }
}
}