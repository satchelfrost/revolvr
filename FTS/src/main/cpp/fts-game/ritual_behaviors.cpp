#include <ritual_behaviors.h>
#include <common.h>

#define RITUAL_BEHAVIOR_CASE_STR(ENUM, NUM) case RitualBehavior::ENUM: return #ENUM;

namespace game {
const char *toString(RitualBehavior behavior) {
    switch (behavior) {
        RITUAL_BEHAVIORS(RITUAL_BEHAVIOR_CASE_STR)
        default:
            return "ComponentType unrecognized";
    }
}

RitualBehavior toRitualBehaviorEnum(const std::string& str) {
    for (int i = 0; i < NUM_BEHAVIORS; i++) {
        std::string enumStr = toString((RitualBehavior)i);
        if (enumStr == str)
            return (RitualBehavior)i;
    }
    THROW(Fmt("No Ritual Behavior found for %s", str.c_str()));
}
}
