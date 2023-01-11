#include "ecs/system/render_system.h"
#include "ecs/system/spatial_system.h"
#include "ecs/ecs.h"

namespace rvr {
std::vector<Spatial *> RenderSystem::GetRenderSpatials() {
    std::vector<Spatial*> spatials;
    auto components = ECS::Instance()->GetComponents(ComponentType::Mesh);
    spatials.reserve(components.size());
    for (auto [eid, component] : components)
        spatials.push_back(ECS::Instance()->GetComponent<Spatial>(eid));
    return spatials;
}
}