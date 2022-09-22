#pragma once

#include "ecs/component/component.h"

namespace rvr {
enum class TrackedSpaceType {
    Origin,
    LeftHand,
    RightHand,
    Nil
};

class TrackedSpace : public Component {
public:
    TrackedSpace();
    TrackedSpaceType trackedSpaceType;
};
}
