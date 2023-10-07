/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include "ecs/entity/entity.h"
#include "component_pool.h"
#include "common.h"
#include <any>
#include <typeindex>

namespace rvr {
class ComponentPoolManager {
public:
    ComponentPoolManager();
    ~ComponentPoolManager();
    void Assign(Entity* entity, Component* component);
    ComponentPool *GetPool(ComponentType cType);

    template<typename T>
    T *GetComponent(type::EntityId id);

private:
    std::vector<ComponentPool*> componentPools_;
    std::map<std::type_index, ComponentType> typeToEnum;
};

// TODO: Change usage: GetComponent<MyComponent> --> GetComponent<MyComponent*>, to make it clear pointer is returned.
/* Warning GetComponent returns nullptr on failure */
template<typename T>
T* ComponentPoolManager::GetComponent(type::EntityId id) {
    auto typeIndex = std::type_index(typeid(T));
    ComponentPool* pool = GetPool(typeToEnum.at(typeIndex));
    return dynamic_cast<T*>(pool->GetComponent(id));
}
}