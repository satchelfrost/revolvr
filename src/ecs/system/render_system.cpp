/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/system/render_system.h>
#include <ecs/system/spatial_system.h>
#include <ecs/component/types/mesh.h>
#include <global_context.h>

namespace rvr::system::render {
void PopulateRenderTransformBuffer(std::vector<math::Transform>& buffer) {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Mesh);
    for (auto [eid, component] : components) {
        auto mesh = dynamic_cast<Mesh*>(component);
        if (mesh->IsVisible()) {
            auto* spatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(eid);
            buffer.push_back(spatial::GetPlayerRelativeTransform(spatial));
        }
    }
    DrawGrid(buffer);
}

void DrawGrid(std::vector<math::Transform>& buffer) {
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