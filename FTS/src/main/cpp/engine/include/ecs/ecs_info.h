#pragma once

#include <pch.h>
#include <ecs/component/component_type.h>

namespace rvr {
namespace constants {
const int MAX_ENTITIES = 30;
const int ROOT_ID = 0;

#define _IMPLEMENTED_COMPONENTS sizeof componentInfo / sizeof componentInfo[0]
constexpr int IMPLEMENTED_COMPONENTS = _IMPLEMENTED_COMPONENTS;
#undef _IMPLEMENTED_COMPONENTS
}

namespace type {
typedef int EntityId;
typedef std::bitset<constants::IMPLEMENTED_COMPONENTS> ComponentMask;
}
}

