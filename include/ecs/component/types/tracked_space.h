#pragma once

#include "ecs/component/component.h"
#include "ecs/component/aux/tracked_space_type.h"

namespace rvr {
class TrackedSpace : public Component {
public:
    TrackedSpace(const TrackedSpace& other) = delete;
    TrackedSpace(const TrackedSpace& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    TrackedSpace(type::EntityId pId, TrackedSpaceType trackedSpaceType);
    const TrackedSpaceType type;
};
}
