#pragma once

#include "ecs/component/component.h"

#define LAST_TRACKED_SPACE 3
#define TRACKED_SPACE_LIST(X) \
    X(Player, 0) \
    X(LeftHand, 1) \
    X(RightHand, 2)           \
    X(Head, LAST_TRACKED_SPACE)           \


#define BUILD_ENUM(ENUM, NUM) ENUM = NUM,

namespace rvr {
namespace constants {
    const int NUM_TRACKED_SPACES = LAST_TRACKED_SPACE + 1;
}

enum class TrackedSpaceType {
    TRACKED_SPACE_LIST(BUILD_ENUM)
    Nil
};

class TrackedSpace : public Component {
public:
    TrackedSpace(type::EntityId pId, TrackedSpaceType trackedSpaceType);
    const TrackedSpaceType type;
};

const char *toString(TrackedSpaceType cType);
TrackedSpaceType toTrackedSpaceTypeEnum(const std::string& str);
}
