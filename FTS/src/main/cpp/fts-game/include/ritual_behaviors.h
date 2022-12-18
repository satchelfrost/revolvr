#pragma once

#include <pch.h>

#define LAST_BEHAVIOR 0
#define RITUAL_BEHAVIORS(X) \
    X(SpinningPointer, LAST_BEHAVIOR) \

#define NUM_BEHAVIORS LAST_BEHAVIOR + 1
#define BUILD_ENUM(ENUM, NUM) ENUM = NUM,

namespace game {
enum class RitualBehavior {
    RITUAL_BEHAVIORS(BUILD_ENUM)
};

const char *toString(RitualBehavior behavior);
RitualBehavior toRitualBehaviorEnum(const std::string &str);
}