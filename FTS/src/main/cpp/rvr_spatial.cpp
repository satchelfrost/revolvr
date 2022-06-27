#include "include/rvr_spatial.h"

RVRSpatial::RVRSpatial(int id) : RVRObject(id, RVRType::RVRSpatial),
                                 visible(true),
                                 pose(RVRMath::Pose::Identity()),
                                 scale(RVRMath::Vector::One()) {}

void RVRSpatial::UniformScale(float scaleFactor) {
    scale.x = scale.x * scaleFactor;
    scale.y = scale.y * scaleFactor;
    scale.z = scale.z * scaleFactor;
}