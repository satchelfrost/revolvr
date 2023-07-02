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

#include <ritual_type.h>
#include <common.h>

namespace rvr {
const char* toString(RitualType cType) {
    int index = (int)cType;
    if (index < 0 || index > constants::IMPLEMENTED_RITUALS)
        return "RitualType unrecognized";
    return ritualInfo[index].str;
}

RitualType toRitualTypeEnum(const std::string& str) {
    for (int i = 0; i < constants::IMPLEMENTED_RITUALS; i++) {
        std::string enumStr = toString((RitualType)i);
        if (enumStr == str)
            return (RitualType)i;
    }
    THROW(Fmt("No ritual type found for %s", str.c_str()));
}
}
