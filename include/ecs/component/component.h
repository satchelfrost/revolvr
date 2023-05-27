#pragma once

#include <ecs/ecs_info.h>

namespace rvr {
class Component {
public:
    Component(ComponentType pType, type::EntityId pId);
    virtual ~Component() = default;

    // Create an identical copy, but with a new entity id
    virtual Component* Clone(type::EntityId newEntityId) = 0;

    const ComponentType type;
    const type::EntityId id;
};
}
