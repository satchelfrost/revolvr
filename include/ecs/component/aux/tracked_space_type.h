/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include <ecs/component/aux/tracked_space_list.h>
#include <pch.h>

namespace rvr {
enum class TrackedSpaceType {
    #define BUILD_ENUM(ENUM) ENUM,
    TRACKED_SPACE_LIST(BUILD_ENUM)
    #undef BUILD_ENUM
};

struct trackedSpaceTypeEnumStrPair { TrackedSpaceType trackedSpaceType; const char* str; };
const trackedSpaceTypeEnumStrPair trackedSpaceInfo[] {
    #define ITEM_STR(NAME) {TrackedSpaceType::NAME, #NAME},
    TRACKED_SPACE_LIST(ITEM_STR)
    #undef ITEM_STR
};

namespace constants {
#define _IMPLEMENTED_TRACKED_SPACES sizeof trackedSpaceInfo / sizeof trackedSpaceInfo[0]
constexpr int IMPLEMENTED_TRACKED_SPACES = _IMPLEMENTED_TRACKED_SPACES;
#undef _IMPLEMENTED_TRACKED_SPACES
}

const char *toString(TrackedSpaceType cType);
TrackedSpaceType toTrackedSpaceTypeEnum(const std::string& str);
}