#include "ecs/component/types/tracked_space.h"
#include "common.h"

namespace rvr {
TrackedSpace::TrackedSpace(type::EntityId pId, TrackedSpaceType trackedSpaceType) :
Component(ComponentType::TrackedSpace, pId), type(trackedSpaceType) {}

Component *TrackedSpace::Clone(type::EntityId newEntityId) {
    return new TrackedSpace(*this, newEntityId);
}

TrackedSpace::TrackedSpace(const TrackedSpace &other, type::EntityId newEntityId) :
Component(ComponentType::TrackedSpace, newEntityId), type(other.type) {}
}
