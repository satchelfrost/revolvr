#pragma once

#include "pch.h"

#define LAST_ENUM 3
#define COMPONENT_LIST(_) \
    _(Spatial, 0) \
    _(Mesh, 1) \
    _(TrackedSpace, 2) \
    _(Ritual, LAST_ENUM)

namespace rvr {
namespace constants {
    const int MAX_ENTITIES = 1000;
    const int IMPLEMENTED_COMPONENTS = LAST_ENUM;
    const int ROOT_ID = 0;
}

#define BUILD_ENUM(ENUM, NUM) ENUM = NUM,
namespace type {
    typedef int EntityId;
    typedef std::bitset<constants::IMPLEMENTED_COMPONENTS> ComponentMask;
}

enum class ComponentType {
    COMPONENT_LIST(BUILD_ENUM)
};
const char* toString(ComponentType cType);
}