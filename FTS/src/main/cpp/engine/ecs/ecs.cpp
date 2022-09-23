#include "ecs/ecs.h"


namespace rvr {
ECS* ECS::instance_ = nullptr;

ECS* ECS::GetInstance() {
    if (!instance_)
        instance_ = new ECS();
    return instance_;
}

EntityPool* ECS::GetEntityPool() {
    return entityPool_;
}

ComponentPoolManager *ECS::GetComponentPoolManager() {
    return componentPoolManager_;
}

void ECS::Init() {
    entityPool_ = new EntityPool();
    componentPoolManager_ = new ComponentPoolManager();
}
}