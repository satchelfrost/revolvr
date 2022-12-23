#pragma once

#include "ecs/component/component.h"
#include <unordered_map>

namespace rvr {
class ComponentPool {
public:
    ComponentPool(ComponentType cType);
    ~ComponentPool();
    Component* GetComponent(type::EntityId id);
    void FreeComponent(type::EntityId id);
    void AssignComponent(Entity* entity, Component* component);
    std::vector<type::EntityId> GetEids();
    std::unordered_map<type::EntityId, Component*> GetComponents();

private:
    std::unordered_map<type::EntityId, Component*> components_;
    ComponentType poolType_;
};
}