#include "include/ecs/component/types/spatial.h"
#include "math/math.h"

namespace rvr {
Spatial::Spatial(type::EntityId pId) : Component(ComponentType::Spatial, pId),
                     pose(math::pose::Identity()),
                     worldPose(math::pose::Identity()),
                     scale(math::vector::One()) {}


void Spatial::UniformScale(float scaleFactor) {
    scale.x = scale.x * scaleFactor;
    scale.y = scale.y * scaleFactor;
    scale.z = scale.z * scaleFactor;
}
}