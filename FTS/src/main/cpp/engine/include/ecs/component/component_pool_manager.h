#pragma once

#include "ecs/ecs.h"
#include "component_pool.h"

namespace rvr {
class ComponentPoolManager {
public:
    ComponentPoolManager();
    ~ComponentPoolManager();
    bool Assign(type::EntityId, ComponentType cType);
private:
    std::vector<ComponentPool*> componentPools_;
};
}