#pragma once

#include "ecs/component/component.h"

namespace rvr {
class ComponentPool {
public:
    ComponentPool(ComponentType cType);
    ~ComponentPool();
    Component* GetComponent(type::EntityId entityId);
    bool CreateComponent(type::EntityId entityId);
    std::vector<Component*> GetComponents() { return components_; }
private:
    std::vector<Component*> components_;
    ComponentType poolType_;
};
}