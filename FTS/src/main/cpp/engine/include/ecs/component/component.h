#pragma once

#include "ecs/component/component_type.h"

namespace rvr {
class Component {
public:
    Component(ComponentType pType);
    virtual ~Component() {}
    const ComponentType type;
};
}
