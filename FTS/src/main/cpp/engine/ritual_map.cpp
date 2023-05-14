#include <ritual_map.h>
#include <common.h>

#define STRING_RITUAL_PAIR(ENUM) {#ENUM, RitualType::ENUM},

namespace rvr {
RitualMap::RitualMap() : rMap_({RITUALS(STRING_RITUAL_PAIR)}){}

RitualType RitualMap::StringToEnum(const std::string& str) {
    try {
        return rMap_.at(str);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("[StringToEnum failed] - RitualType %s unrecognized", str.c_str()));
    }
}

const char* RitualMap::EnumToString(RitualType type) {
    for (auto &[key, value]: rMap_)
        if (value == type)
            return key.c_str();
    THROW(Fmt("[EnumToString failed] - RitualType %d unrecognized", type));
}
}
