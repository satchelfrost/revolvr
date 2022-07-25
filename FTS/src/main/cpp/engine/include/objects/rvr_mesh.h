#pragma once

#include "rvr_spatial.h"
#include "pch.h"

class RVRMesh : public RVRSpatial {
public:
    RVRMesh(int id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
};