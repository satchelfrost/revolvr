#pragma once

#include "ecs/ecs.h"

namespace rvr {
class Component {
public:
    Component(ComponentType pType);
    virtual ~Component() {}
    const ComponentType type;
};
}
