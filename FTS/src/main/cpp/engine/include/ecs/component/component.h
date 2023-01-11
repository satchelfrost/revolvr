#pragma once

#include "ecs/component/component_type.h"

namespace rvr {
class Component {
public:
    Component(ComponentType pType, type::EntityId pId);
    virtual ~Component() {}
    const ComponentType type;
    const type::EntityId id;
};
}
