#include "ecs/entity/entity_factory.h"
#include "ecs/ecs.h"

namespace rvr {
Entity *EntityFactory::CreateEntity(const std::vector<ComponentType>& cTypes) {
    Entity* entity = ECS::Instance()->GetNewEntity(cTypes);
    for (auto cType : cTypes)
        ECS::Instance()->Assign(entity->id, cType);
    return entity;
}
}
