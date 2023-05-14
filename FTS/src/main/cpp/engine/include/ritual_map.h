#pragma once

#include <pch.h>
#include <rituals.h>

#define BUILD_RITUAL_TYPE_ENUM(ENUM) ENUM,

namespace rvr {
enum class RitualType {
    RITUALS(BUILD_RITUAL_TYPE_ENUM)
};

class RitualMap {
    const std::map<std::string, RitualType> rMap_;
public:
    RitualMap();
    RitualType StringToEnum(const std::string& str);
    const char* EnumToString(RitualType type);
};
}