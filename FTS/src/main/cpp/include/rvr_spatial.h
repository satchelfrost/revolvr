#pragma once

#include "rvr_object.h"
#include "rvr_math.h"
#include "pch.h"

class RVRSpatial : public RVRObject {
public:
    RVRSpatial(int id);
    void UniformScale(float scaleFactor);
    bool visible;
    XrPosef pose;
    XrVector3f scale;
};