#include "ecs/entity/entity_factory.h"

namespace rvr {
EntityFactory::EntityFactory(EntityPool* entityPool, ComponentPoolManager* componentPoolManager) :
entityPool_(entityPool), componentPoolManager_(componentPoolManager) {}

Entity *EntityFactory::CreateEntity(const std::vector<ComponentType>& cTypes) {
    Entity* entity = entityPool_->GetNextEntity(cTypes);
    for (auto cType : cTypes)
        if (!componentPoolManager_->Assign(entity->id, cType))
            Log::Write(Log::Level::Warning,
                       Fmt("Entity %d could not be assigned component %s", entity->id, toString(cType)));
    return entity;
}
}
