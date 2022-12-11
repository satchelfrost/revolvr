#pragma once

#include "pch.h"

#define LAST_ENUM 3
#define COMPONENT_LIST(X) \
    X(Spatial, 0) \
    X(Mesh, 1) \
    X(TrackedSpace, 2) \
    X(Ritual, LAST_ENUM) // TODO: instead of having a last enum just create a dummy enum

namespace rvr {
namespace constants {
    const int MAX_ENTITIES = 30;
    const int IMPLEMENTED_COMPONENTS = LAST_ENUM + 1;
    const int ROOT_ID = 0;
}

namespace type {
    typedef int EntityId;
    typedef std::bitset<constants::IMPLEMENTED_COMPONENTS> ComponentMask;
}
}

