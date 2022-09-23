#pragma once

#include "pch.h"
#include "ecs/component/component_type.h"

namespace rvr {
class Entity {
public:
    Entity(int entityId, const std::vector<ComponentType>& cTypes);
    std::vector<ComponentType> GetComponentTypes();
    void InitMask(const std::vector<ComponentType>& cTypes);
    void ResetMask();
    bool HasComponent(ComponentType cType);
    void SetName(std::string name);
    std::string GetName();
    const int id;
private:
    void GenerateAndSetName();
    std::string name_;
    type::ComponentMask mask_;
};
}