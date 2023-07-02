/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#include "ecs/ecs.h"

namespace rvr {
ECS::ECS() {
    entityPool_ = new EntityPool();
    componentPoolManager_ = new ComponentPoolManager();
}

std::map<type::EntityId, Component*> ECS::GetComponents(ComponentType cType) {
    return GetPool(cType)->GetComponents();
}

ComponentPool *ECS::GetPool(ComponentType cType) {
    return componentPoolManager_->GetPool(cType);
}

Entity *ECS::CreateNewEntity(type::EntityId id, bool setRootAsParent) {
    return entityPool_->CreateNewEntity(id, setRootAsParent);
}

Entity *ECS::CreateNewEntity(bool setRootAsParent) {
    return entityPool_->CreateNewEntity(setRootAsParent);
}

void ECS::Assign(Entity* entity, Component* component) {
    componentPoolManager_->Assign(entity, component);
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

void ECS::FillHoles() {
    entityPool_->FillHoles();
}
}