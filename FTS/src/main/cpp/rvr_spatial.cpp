#include "include/rvr_spatial.h"

RVRSpatial::RVRSpatial(int id) : RVRObject(id, RVRType::RVRSpatial),
                                 pose(RVRMath::Pose::Identity()),
                                 scale(RVRMath::Vector::Zero()),
                                 visible(true) {}