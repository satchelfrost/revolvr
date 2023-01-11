#pragma once

#include "ecs/entity/entity_pool.h"
#include "ecs/component/component_type.h"

namespace rvr {
class EntityFactory {
public:
    static Entity* CreateEntity();
    static Entity* CreateEntity(type::EntityId id);
};
}