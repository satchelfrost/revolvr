#pragma once

#include <pch.h>

#define LAST_RITUAL 2
#define RITUALS(X) \
    X(SpinningPointer, 0) \
    X(HandBehavior, 1) \
    X(MovingSoundBox, LAST_RITUAL) \

#define NUM_RITUALS LAST_RITUAL + 1
#define BUILD_ENUM(ENUM, NUM) ENUM = NUM,

namespace game {
enum class RitualType {
    RITUALS(BUILD_ENUM)
};

const char *toString(RitualType type);
RitualType toRitualTypeEnum(const std::string &str);
}