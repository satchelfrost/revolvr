#include "ecs/component/component_pool.h"
#include "check.h"
#include "logger.h"
#include "ecs/component/all_components.h"

#define ALLOC_CASE_STR(TYPE, NUM) case ComponentType::TYPE: component = new TYPE(); break;

namespace rvr {
ComponentPool::ComponentPool(ComponentType cType) : poolType_(cType) {}

ComponentPool::~ComponentPool() {
    for (auto& component : components_)
        delete component.second;
}

void ComponentPool::CreateComponent(type::EntityId id) {
    Component* component = nullptr;
    switch (poolType_) {
        COMPONENT_LIST(ALLOC_CASE_STR)
        default:
            break;
    }
    CHECK_MSG(component,Fmt("Entity %d could not be created, component %s has no implementation",
                            toString(ComponentType(poolType_))));
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