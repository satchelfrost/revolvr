#include "ecs/component/types/tracked_space.h"


namespace rvr {
TrackedSpace::TrackedSpace() :
Component(ComponentType::TrackedSpace), trackedSpaceType(TrackedSpaceType::Nil) {}
}
