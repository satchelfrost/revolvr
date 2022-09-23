#pragma once

#include "ecs/entity/entity_pool.h"
#include "ecs/component/component_pool_manager.h"


namespace rvr {
class ECS {
public:
    void Init();
    static ECS* GetInstance();
    EntityPool* GetEntityPool();
    ComponentPoolManager* GetComponentPoolManager();

    template<typename T>
    T *GetComponent(Entity *entity);

private:
    static ECS* instance_;
    EntityPool* entityPool_;
    ComponentPoolManager* componentPoolManager_;
};

template<typename T>
T* ECS::GetComponent(Entity *entity) {
    auto cpm = GetComponentPoolManager();
    return cpm->GetComponent<T>(entity);
}
}