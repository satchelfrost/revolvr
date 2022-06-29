#pragma once

#include "rvr_object.h"
#include "rvr_math.h"

class RVRSpatial : public RVRObject {
public:
    RVRSpatial(int id);
    XrPosef pose;
    XrVector3f scale;
    bool visible;
};