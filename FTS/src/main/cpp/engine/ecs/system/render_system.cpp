#include "ecs/system/render_system.h"
#include "ecs/ecs.h"

namespace rvr {
std::vector<Entity*> RenderSystem::GatherRenderables() {
    std::vector<Entity*> entities;
    EntityPool* entityPool = rvr::ECS::GetInstance()->GetEntityPool();
    for (auto& entity : entityPool->GetEntities()) {
        if (!entity)
            continue;
        if (entity->HasComponent(rvr::ComponentType::Mesh))
            entities.push_back(entity);
    }
    return entities;
}
}