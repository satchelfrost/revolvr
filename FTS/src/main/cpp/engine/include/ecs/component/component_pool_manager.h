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
    void Assign(type::EntityId, Component* component);
    ComponentPool *GetPool(ComponentType cType);
    Component* GetComponent(type::EntityId id, ComponentType cType);

    template<typename T>
    T *GetComponent(type::EntityId id);

private:
    std::vector<ComponentPool*> componentPools_;
    std::map<std::type_index, ComponentType> typeToEnum;
};

template<typename T>
T* ComponentPoolManager::GetComponent(type::EntityId id) {
    auto typeIndex = std::type_index(typeid(T));
    ComponentPool* pool = GetPool(typeToEnum.at(typeIndex));
    T* componentType = dynamic_cast<T*>(pool->GetComponent(id));
    CHECK_MSG(componentType, Fmt("GetComponent() on entity id %d cast to nullptr", id));
    return componentType;
}
}