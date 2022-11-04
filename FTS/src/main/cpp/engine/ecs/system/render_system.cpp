#include "ecs/system/render_system.h"
#include "ecs/system/spatial_system.h"
#include "ecs/ecs.h"

namespace rvr {
std::vector<Spatial *> RenderSystem::GetRenderSpatials() {
    std::vector<Spatial*> spatials;
    for (auto id : ECS::Instance()->GetEids(ComponentType::Mesh)) {
        SpatialSystem::CalculateWorldPosition(id);
        spatials.push_back(ECS::Instance()->GetComponent<Spatial>(id));
    }
    return spatials;
}
}