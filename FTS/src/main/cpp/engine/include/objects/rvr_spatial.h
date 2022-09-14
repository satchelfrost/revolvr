#pragma once

#include "pch.h"
#include "rvr_object.h"
#include "math/rvr_math.h"

class RVRSpatial : public RVRObject {
public:
    RVRSpatial(int id);
    RVRSpatial(int id, RVRType type);
    void UniformScale(float scaleFactor);

    virtual void Begin() override;
    virtual void Update(float delta) override;

    bool visible;
    XrPosef pose;
    XrPosef worldPose;
    XrVector3f scale;
};