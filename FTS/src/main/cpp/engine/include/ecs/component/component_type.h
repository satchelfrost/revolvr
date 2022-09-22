#pragma once

#include "ecs/ecs.h"
#include <map>
#include <typeindex>

#define BUILD_ENUM(ENUM, NUM) ENUM = NUM,

namespace rvr {
enum class ComponentType {
    COMPONENT_LIST(BUILD_ENUM)
};

const char* toString(ComponentType cType);

static std::map<std::type_index, ComponentType> typeToEnum;
}