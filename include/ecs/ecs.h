/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include "ecs/entity/entity_pool.h"
#include "ecs/component/component_pool_manager.h"

namespace rvr {
class ECS {
public:
    ECS();
    void Assign(Entity* entity, Component* component);
    void FreeEntity(type::EntityId id);
    std::map<type::EntityId, Component*> GetComponents(ComponentType cType);
    ComponentPool* GetPool(ComponentType cType);
    std::vector<type::EntityId> GetEids(ComponentType cType);
    Entity* CreateNewEntity(type::EntityId id, bool setRootAsParent = true);
    Entity* CreateNewEntity(bool setRootAsParent = true);
    Entity* GetEntity(type::EntityId id);
    void FillHoles();

    template<typename T>
    T* GetComponent(type::EntityId id);

    template<typename T, typename U>
    std::pair<T*, U*> GetComponentPair(type::EntityId id);

private:
    EntityPool* entityPool_;
    ComponentPoolManager* componentPoolManager_;
};

template<typename T>
T* ECS::GetComponent(type::EntityId id) {
    return componentPoolManager_->GetComponent<T>(id);
}

template<typename T, typename U>
std::pair<T*, U*> ECS::GetComponentPair(type::EntityId id) {
    return std::make_pair(GetComponent<T>(id), GetComponent<U>(id));
}
}