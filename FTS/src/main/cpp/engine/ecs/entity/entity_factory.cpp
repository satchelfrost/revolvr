#include "ecs/entity/entity_factory.h"
#include "ecs/ecs.h"

namespace rvr {
Entity *EntityFactory::CreateEntity() {
    Entity* entity = ECS::Instance()->CreateNewEntity();
    return entity;
}

Entity *EntityFactory::CreateEntity(type::EntityId id) {
    Entity* entity = ECS::Instance()->CreateNewEntity(id);
    return entity;
}
}
