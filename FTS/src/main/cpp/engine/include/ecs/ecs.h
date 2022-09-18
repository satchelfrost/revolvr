#pragma once

#include "pch.h"

namespace rvr {
namespace constants {
    const int MAX_ENTITIES = 1000;
    const int IMPLEMENTED_COMPONENTS = 4;
    const int ROOT_ID = 0;
}

namespace type {
    typedef int EntityId;
    typedef std::bitset<constants::IMPLEMENTED_COMPONENTS> ComponentMask;
}

enum class ComponentType {
    Spatial = 0,
    Mesh = 1,
    Origin = 2,
    Hand = constants::IMPLEMENTED_COMPONENTS - 1
};
const char* toString(ComponentType cType);
}