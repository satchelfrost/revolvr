#pragma once

#include "pch.h"
#include "ecs/component/component_type.h"

namespace rvr {
class Entity {
public:
    // General Info methods
    Entity(int entityId, const std::vector<ComponentType>& cTypes);
    std::vector<ComponentType> GetComponentTypes();
    void InitMask(const std::vector<ComponentType>& cTypes);
    void ResetMask();
    bool HasComponent(ComponentType cType);
    void SetName(std::string name);
    std::string GetName();

    // TODO: Destroy(), needs reference to entity pool

    // Tree related methods
    void AddChild(Entity* child);
    void SetParent(Entity* parent);
    Entity* GetParent();
    std::list<Entity*>& GetChildren();

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