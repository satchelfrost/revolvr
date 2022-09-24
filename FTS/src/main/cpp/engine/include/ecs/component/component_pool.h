#pragma once

#include "ecs/component/component.h"
#include <unordered_map>

namespace rvr {
class ComponentPool {
public:
    ComponentPool(ComponentType cType);
    ~ComponentPool();
    Component* GetComponent(type::EntityId id);
    void CreateComponent(type::EntityId id);
    std::vector<type::EntityId> GetEids();
private:
    std::unordered_map<type::EntityId, Component*> components_;
    ComponentType poolType_;
};
}