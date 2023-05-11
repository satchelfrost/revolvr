#include <include/ecs/entity/entity.h>
#include <ecs/component/component_pool.h>
#include <common.h>

namespace rvr {
ComponentPool::ComponentPool(ComponentType cType) : poolType_(cType) {}

ComponentPool::~ComponentPool() {
    for (auto& component : components_)
        delete component.second;
}

void ComponentPool::AssignComponent(Entity* entity, Component* component) {
    if (components_.find(entity->id) != components_.end()) {
        THROW(Fmt("Component %s associated with id %d already exists with type %s", toString(component->type),
                  entity->id, toString(components_.at(entity->id)->type)));
    }
    entity->AddComponent(component->type);
    components_.emplace(entity->id, component);
}

Component *ComponentPool::GetComponent(type::EntityId id) {
    try {
        return components_.at(id);
    }
    catch (std::out_of_range& e) {
        Log::Write(Log::Level::Warning, Fmt("Component does not exist for id %d", id));
        return nullptr;
    }
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

std::map<type::EntityId, Component *> ComponentPool::GetComponents() {
    return components_;
}
}