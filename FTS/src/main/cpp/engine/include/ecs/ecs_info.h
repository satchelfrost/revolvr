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
    const int MAX_ENTITIES = 10;
    const int IMPLEMENTED_COMPONENTS = LAST_ENUM + 1;
    const int ROOT_ID = 0;
}

namespace type {
    typedef int EntityId;
    typedef std::bitset<constants::IMPLEMENTED_COMPONENTS> ComponentMask;
}
}

