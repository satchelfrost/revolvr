#pragma once

#include "pch.h"
#include "check.h"
#include "ecs/ecs.h"

namespace rvr {
class Entity {
public:
    Entity(int entityId, const std::vector<ComponentType>& cTypes);
    std::vector<ComponentType> GetComponentTypes();
    void InitMask(const std::vector<ComponentType>& cTypes);
    void ResetMask();
    const int id;
private:
    type::ComponentMask mask_;
};
}