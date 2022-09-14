#pragma once

#include <objects/rvr_mesh.h>
#include <math/xr_linear.h>

class SpinningPointer : public RVRMesh {
public:
    SpinningPointer(int id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
private:
    int turnAmt_;
    int turnSpeed_;
    int transDirection_;
};