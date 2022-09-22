#pragma once

#include "ecs/ecs.h"
#include "ecs/entity/entity.h"
#include "component_pool.h"

namespace rvr {
class ComponentPoolManager {
public:
    ComponentPoolManager();
    ~ComponentPoolManager();
    bool Assign(type::EntityId, ComponentType cType);
    ComponentPool* GetPool(ComponentType cType);
    Component* GetComponent(Entity* entity, ComponentType cType);
    std::vector<Component*> GetComponents(Entity* entity);
private:
    std::vector<ComponentPool*> componentPools_;
};
}