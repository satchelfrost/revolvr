#include "include/ecs/component/types/spatial.h"
#include <math/math.h>

namespace rvr {
Spatial::Spatial(type::EntityId pId) :
                     Component(ComponentType::Spatial, pId),
                     transform(math::Transform::Identity()),
                     worldTransform(math::Transform::Identity()) {}
}