#include "ecs/component/types/tracked_space.h"
#include "common.h"

#define TRACKED_SPACE_CASE_STR(ENUM, NUM) case TrackedSpaceType::ENUM: return #ENUM;

namespace rvr {
const char *toString(TrackedSpaceType cType) {
    switch (cType) {
        TRACKED_SPACE_LIST(TRACKED_SPACE_CASE_STR)
        default:
            return "ComponentType unrecognized";
    }
}

TrackedSpaceType toTrackedSpaceTypeEnum(const std::string& str) {
    for (int i = 0; i < constants::NUM_TRACKED_SPACES; i++) {
        std::string enumStr = toString((TrackedSpaceType)i);
        if (enumStr == str)
            return (TrackedSpaceType)i;
    }
    THROW(Fmt("No tracked space found for %s", str.c_str()))
}

TrackedSpace::TrackedSpace(type::EntityId pId, TrackedSpaceType trackedSpaceType) :
Component(ComponentType::TrackedSpace, pId), type(trackedSpaceType) {}

Component *TrackedSpace::Clone(type::EntityId newEntityId) {
    return new TrackedSpace(newEntityId, this->type);
}
}
