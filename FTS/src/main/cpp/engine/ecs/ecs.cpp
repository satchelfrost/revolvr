#include "ecs/ecs.h"


namespace rvr {
ECS* ECS::instance_ = nullptr;

ECS* ECS::Instance() {
    if (!instance_)
        instance_ = new ECS();
    return instance_;
}

void ECS::Init() {
    entityPool_ = new EntityPool();
    componentPoolManager_ = new ComponentPoolManager();
}

ComponentPool *ECS::GetPool(ComponentType cType) {
    return componentPoolManager_->GetPool(cType);
}

Entity *ECS::GetNewEntity(const std::vector<ComponentType>& cTypes) {
    return entityPool_->GetNewEntity(cTypes);
}

void ECS::Assign(type::EntityId id, ComponentType cType) {
    componentPoolManager_->Assign(id, cType);
}

void ECS::FreeEntity(type::EntityId id) {
    entityPool_->FreeEntity(id);
}


std::vector<type::EntityId> ECS::GetEids(ComponentType cType) {
    return GetPool(cType)->GetEids();
}

Entity *ECS::GetEntity(type::EntityId id) {
    return entityPool_->GetEntity(id);
}
}