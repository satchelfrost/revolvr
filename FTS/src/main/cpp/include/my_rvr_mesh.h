#pragma once

#include "rvr_mesh.h"

class MyRVRMesh : public RVRMesh {
public:
    MyRVRMesh(int id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
};