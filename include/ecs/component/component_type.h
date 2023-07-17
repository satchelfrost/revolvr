/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <ecs/component/component_list.h>

namespace rvr {
enum class ComponentType {
    #define BUILD_ENUM(ENUM) ENUM,
    COMPONENT_LIST(BUILD_ENUM)
    #undef BUILD_ENUM
};

struct componentTypeEnumStrPair { ComponentType componentType; const char* str; };
const componentTypeEnumStrPair componentInfo[] {
    #define ITEM_STR(NAME) {ComponentType::NAME, #NAME},
    COMPONENT_LIST(ITEM_STR)
    #undef ITEM_STR
};

const char* toString(ComponentType cType);
ComponentType toComponentTypeEnum(const std::string& str);
}