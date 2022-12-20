#pragma once

#include "ecs/entity/entity_pool.h"
#include "ecs/component/component_pool_manager.h"


namespace rvr {
class ECS {
public:
    void Init();
    static ECS* Instance();
    void Assign(type::EntityId id, Component* cType);
    void FreeEntity(type::EntityId id);
    ComponentPool* GetPool(ComponentType cType);
    std::vector<type::EntityId> GetEids(ComponentType cType);
    Entity* CreateNewEntity(type::EntityId id);
    Entity* CreateNewEntity();
    Entity* GetEntity(type::EntityId id);
    void FillHoles();

    template<typename T>
    T* GetComponent(type::EntityId id);

    template<typename T, typename U>
    std::pair<T*, U*> GetComponentPair(type::EntityId id);

private:
    static ECS* instance_;
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