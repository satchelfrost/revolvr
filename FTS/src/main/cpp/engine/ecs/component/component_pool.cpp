#include "ecs/component/component_pool.h"
#include "check.h"
#include "logger.h"
#include "ecs/component/all_components.h" // TODO: Check if this needs to be here

namespace rvr {
ComponentPool::ComponentPool(ComponentType cType) : poolType_(cType) {}

ComponentPool::~ComponentPool() {
    for (auto& component : components_)
        delete component.second;
}

void ComponentPool::AssignComponent(type::EntityId id, Component* component) {
    if (components_.find(id) != components_.end())
        THROW(Fmt("Component associated with id %d already exists"))

    components_.emplace(id, component);
}

Component *ComponentPool::GetComponent(type::EntityId id) {
    return components_.at(id);
}

std::vector<type::EntityId> ComponentPool::GetEids() {
    std::vector<type::EntityId> eids_;
    for (auto item : components_)
        eids_.push_back(item.first);
    return eids_;
}

void ComponentPool::FreeComponent(type::EntityId id) {
    Component* component = GetComponent(id);
    delete component;
    components_.erase(id);
}
}