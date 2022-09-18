#pragma once


#include "ecs/ecs.h"
#include "component.h"
#include "check.h"
#include "logger.h"

// BLARG - CODE GENERATION START
#include "spatial.h"
// BLARG - CODE GENERATION END

namespace rvr {
class ComponentPool {
public:
    ComponentPool(ComponentType cType);
    ~ComponentPool();
    bool CreateComponent(type::EntityId entityId);
private:
    std::vector<Component*> components_;
    ComponentType poolType_;
};
}