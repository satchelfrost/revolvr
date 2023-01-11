#include <ritual_type.h>
#include <common.h>

#define RITUAL_TYPE_CASE_STR(ENUM, NUM) case RitualType::ENUM: return #ENUM;

namespace game {
const char *toString(RitualType rType) {
    switch (rType) {
        RITUALS(RITUAL_TYPE_CASE_STR)
        default:
            return "RitualType unrecognized";
    }
}

RitualType toRitualTypeEnum(const std::string& str) {
    for (int i = 0; i < NUM_RITUALS; i++) {
        std::string enumStr = toString((RitualType)i);
        if (enumStr == str)
            return (RitualType)i;
    }
    THROW(Fmt("No RitualType found for %s", str.c_str()));
}
}
