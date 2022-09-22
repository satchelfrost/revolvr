#pragma once

#include "check.h"
#include "logger.h"
#include "ecs/component/all_components.h"

namespace rvr {
class ComponentPool {
public:
    ComponentPool(ComponentType cType);
    ~ComponentPool();
    Component* GetComponent(type::EntityId entityId);
    bool CreateComponent(type::EntityId entityId);
private:
    std::vector<Component*> components_;
    ComponentType poolType_;
};
}