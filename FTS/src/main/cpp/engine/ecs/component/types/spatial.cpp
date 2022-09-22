#include "include/ecs/component/types/Spatial.h"

namespace rvr {
Spatial::Spatial() : Component(ComponentType::Spatial),
                               pose(RVRMath::Pose::Identity()),
                               worldPose(RVRMath::Pose::Identity()),
                               scale(RVRMath::Vector::One()) {}

void Spatial::UniformScale(float scaleFactor) {
    scale.x = scale.x * scaleFactor;
    scale.y = scale.y * scaleFactor;
    scale.z = scale.z * scaleFactor;
}
}