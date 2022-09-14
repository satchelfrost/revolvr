#pragma once

#include "pch.h"
#include "rvr_spatial.h"

class RVRMesh : public RVRSpatial {
public:
    RVRMesh(int id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
};