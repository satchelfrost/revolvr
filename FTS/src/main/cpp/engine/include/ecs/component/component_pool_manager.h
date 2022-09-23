#pragma once

#include "ecs/entity/entity.h"
#include "component_pool.h"
#include "common.h"
#include <any>

namespace rvr {
class ComponentPoolManager {
public:
    ComponentPoolManager();
    ~ComponentPoolManager();
    bool Assign(type::EntityId, ComponentType cType);
    ComponentPool *GetPool(ComponentType cType);
    Component* GetComponent(Entity *entity, ComponentType cType);
    std::vector<Component*> GetAllComponents(Entity *entity);

    template<typename... Ts>
    std::vector<Component*> GetComponents(Entity *entity);

    template<typename T>
    T *GetComponent(Entity *entity);

private:
    std::vector<ComponentPool*> componentPools_;
    std::map<std::type_index, ComponentType> typeToEnum;
};

template<typename T>
T* ComponentPoolManager::GetComponent(Entity *entity) {
    auto typeIndex = std::type_index(typeid(T));
    ComponentPool* pool = GetPool(typeToEnum.at(typeIndex));
    T* componentType = dynamic_cast<T*>(pool->GetComponent(entity->id));
    if (!componentType) {
        Log::Write(Log::Level::Warning,
                Fmt("GetComponent() on entity id %d cast to nullptr", entity->id));
    }
    return componentType;
}

template<typename... Ts>
std::vector<Component*> ComponentPoolManager::GetComponents(Entity *entity) {
    std::vector<Component*> components;
    std::vector<std::type_index> indexes = {std::type_index(typeid(Ts)...)};
    for (auto& index : indexes)
        components.push_back(GetPool(typeToEnum.at(index))->GetComponent(entity->id));
    return components;
}
}