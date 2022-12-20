#pragma once

#include "pch.h"
#include "ecs/component/component_type.h"

namespace rvr {
class Entity {
public:
    // General Info methods
    Entity(int entityId);
    std::vector<ComponentType> GetComponentTypes();
    void InitMask(const std::vector<ComponentType>& cTypes);
    void SetMask(int bit);
    void ResetMask();
    bool HasComponent(ComponentType cType);
    bool Active();
    void SetName(std::string name);
    std::string GetName();


    // Tree related methods
    void AddChild(Entity* child);
    void SetParent(Entity* parent);
    Entity* GetParent();
    std::list<Entity*>& GetChildren();
    void Destroy();
    void FreeComponents();

    const int id;

private:
    void GenerateAndSetName();
    void RemoveFromParent();

    Entity* parent_;
    std::list<Entity*> children_;
    std::string name_;
    type::ComponentMask mask_;
};
}