#pragma once

#include "rvr_object.h"
#include "rvr_math.h"
#include "pch.h"

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