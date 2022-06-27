#pragma once

#include "rvr_spatial.h"

class MyRVRSpatial : public RVRSpatial {
public:
    MyRVRSpatial(int id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
};