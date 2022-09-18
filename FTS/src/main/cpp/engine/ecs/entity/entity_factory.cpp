#include "ecs/entity/entity_factory.h"

namespace rvr {
EntityFactory::EntityFactory(EntityPool* entityPool, ComponentPoolManager* componentPoolManager) :
entityPool_(entityPool), componentPoolManager_(componentPoolManager) {}

Entity *EntityFactory::CreateEntity(const std::vector<ComponentType>& cTypes) {
    Entity* entity = entityPool_->GetNextEntity(cTypes);
    for (auto cType : cTypes)
        componentPoolManager_->Assign(entity->id, cType);
    return entity;
}
}
