/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include "pch.h"
#include <ecs/ecs_info.h>

namespace rvr {
class Entity {
public:
    // General Info methods
    Entity(int entityId);
    std::vector<ComponentType> GetComponentTypes();
    void AddComponent(ComponentType cType);
    void ResetMask();
    bool HasComponent(ComponentType cType);
    bool Active();
    void SetName(std::string name);
    std::string GetName();
    Entity* Clone(type::EntityId newEntityId);
    Entity* Clone();


    // Tree related methods
    void AddChild(Entity* child);
    Entity* GetParent();
    std::list<Entity*>& GetChildren();
    void Destroy();
    void FreeComponents();

    const int id;

private:
    void SetParent(Entity* parent);
    void DestroyRecursive();
    void GenerateAndSetName();
    void RemoveFromParent();

    Entity* parent_;
    std::list<Entity*> children_;
    std::string name_;
    type::ComponentMask mask_;
};
}