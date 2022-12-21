#pragma once

#include <math/xr_linear.h>
#include <ecs/ecs_info.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/ritual.h>

class SpinningPointer : public rvr::Ritual {
public:
    SpinningPointer(rvr::type::EntityId id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
private:
    rvr::Spatial* spatial_;
    int turnAmt_;
    int turnSpeed_;
    int transDirection_;
};