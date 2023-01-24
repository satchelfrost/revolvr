#pragma once

#include "pch.h"

#define LAST_COMPONENT 4
#define COMPONENT_LIST(X) \
    X(Spatial, 0) \
    X(Mesh, 1) \
    X(TrackedSpace, 2) \
    X(Ritual, 3) \
    X(Collider, LAST_COMPONENT)

namespace rvr {
namespace constants {
    const int MAX_ENTITIES = 30;
    const int IMPLEMENTED_COMPONENTS = LAST_COMPONENT + 1;
    const int ROOT_ID = 0;
}

namespace type {
    typedef int EntityId;
    typedef std::bitset<constants::IMPLEMENTED_COMPONENTS> ComponentMask;
}
}

