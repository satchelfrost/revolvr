#include "ecs/entity/entity_factory.h"
#include "ecs/ecs.h"

namespace rvr {
Entity *EntityFactory::CreateEntity(const std::vector<ComponentType>& cTypes) {
    Entity* entity = ECS::GetInstance()->GetEntityPool()->GetNextEntity(cTypes);
    for (auto cType : cTypes)
        if (!ECS::GetInstance()->GetComponentPoolManager()->Assign(entity->id, cType))
            Log::Write(Log::Level::Warning,
                       Fmt("Entity %d could not be assigned component %s", entity->id, toString(cType)));
    return entity;
}
}
