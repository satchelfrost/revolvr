#pragma once

#include "ecs/ecs.h"
#include "ecs/entity/entity_pool.h"
#include "ecs/component/component_pool_manager.h"

namespace rvr {
class EntityFactory {
public:
    EntityFactory(EntityPool* entityPool, ComponentPoolManager* componentPoolManager);
    Entity* CreateEntity(const std::vector<ComponentType>& cTypes);
private:
    EntityPool* entityPool_;
    ComponentPoolManager* componentPoolManager_;
};
}