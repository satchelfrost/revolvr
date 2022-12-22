#include "include/ecs/component/types/spatial.h"
#include "math/rvr_math.h"

namespace rvr {
Spatial::Spatial(type::EntityId pId) : Component(ComponentType::Spatial, pId),
                     pose(RVRMath::Pose::Identity()),
                     worldPose(RVRMath::Pose::Identity()),
                     scale(RVRMath::Vector::One()) {}


void Spatial::UniformScale(float scaleFactor) {
    scale.x = scale.x * scaleFactor;
    scale.y = scale.y * scaleFactor;
    scale.z = scale.z * scaleFactor;
}
}