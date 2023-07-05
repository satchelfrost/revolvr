/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <ecs/component/aux/tracked_space_type.h>
#include <common.h>

namespace rvr {
const char* toString(TrackedSpaceType tsType) {
    int index = (int)tsType;
    if (index < 0 || index > constants::IMPLEMENTED_TRACKED_SPACES)
        return "Tracked space type unrecognized";
    return trackedSpaceInfo[index].str;
}

TrackedSpaceType toTrackedSpaceTypeEnum(const std::string& str) {
    for (int i = 0; i < constants::IMPLEMENTED_TRACKED_SPACES; i++) {
        std::string enumStr = toString((TrackedSpaceType)i);
        if (enumStr == str)
            return (TrackedSpaceType)i;
    }
    THROW(Fmt("No tracked space type found for %s", str.c_str()));
}
}
