/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include "ritual_list.h"

namespace rvr {
enum class RitualType {
    #define BUILD_ENUM(ENUM) ENUM,
    RITUAL_LIST(BUILD_ENUM)
    #undef BUILD_ENUM
};

struct ritualTypeEnumStrPair { RitualType ritualType; const char* str; };
const ritualTypeEnumStrPair ritualInfo[] {
    #define ITEM_STR(NAME) {RitualType::NAME, #NAME},
    RITUAL_LIST(ITEM_STR)
    #undef ITEM_STR
};

namespace constants {
#define _IMPLEMENTED_RITUALS sizeof ritualInfo / sizeof ritualInfo[0]
constexpr int IMPLEMENTED_RITUALS = _IMPLEMENTED_RITUALS;
#undef _IMPLEMENTED_RITUALS
}

const char* toString(RitualType cType);
RitualType toRitualTypeEnum(const std::string& str);
}