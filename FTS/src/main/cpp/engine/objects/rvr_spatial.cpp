#include "include/objects/rvr_spatial.h"

RVRSpatial::RVRSpatial(int id) : RVRObject(id, RVRType::Spatial),
                                 visible(true),
                                 pose(RVRMath::Pose::Identity()),
                                 worldPose(RVRMath::Pose::Identity()),
                                 scale(RVRMath::Vector::One()) {}

RVRSpatial::RVRSpatial(int id, RVRType rvrType) : RVRObject(id, rvrType),
                                                  visible(true),
                                                  pose(RVRMath::Pose::Identity()),
                                                  worldPose(RVRMath::Pose::Identity()),
                                                  scale(RVRMath::Vector::One()) {}

void RVRSpatial::UniformScale(float scaleFactor) {
    scale.x = scale.x * scaleFactor;
    scale.y = scale.y * scaleFactor;
    scale.z = scale.z * scaleFactor;
}

void RVRSpatial::Begin() {}
void RVRSpatial::Update(float delta) {}
