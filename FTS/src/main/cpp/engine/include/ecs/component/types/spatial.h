#pragma once

#include "ecs/component/component.h"
#include <math/transform.h>

namespace rvr {
class Spatial : public Component {
public:
    Spatial(type::EntityId pId);

    math::Transform transform;
    math::Transform worldTransform;
};
}