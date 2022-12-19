#include "ecs/entity/entity_factory.h"
#include "ecs/ecs.h"

namespace rvr {
Entity *EntityFactory::CreateEntity(const std::vector<ComponentType>& cTypes) {
    Entity* entity = ECS::Instance()->CreateNewEntity(cTypes);
    for (auto cType : cTypes)
        ECS::Instance()->Assign(entity->id, cType);
    return entity;
}

Entity *EntityFactory::CreateEntity(type::EntityId id, const std::vector<ComponentType>& cTypes) {
    Entity* entity = ECS::Instance()->CreateNewEntity(id, cTypes);
    for (auto cType : cTypes)
        ECS::Instance()->Assign(entity->id, cType);
    return entity;
}
}
