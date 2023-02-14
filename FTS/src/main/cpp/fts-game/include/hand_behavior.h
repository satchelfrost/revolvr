#pragma once

#include <math/xr_linear.h>
#include <ecs/ecs_info.h>
#include <ecs/component/types/ritual.h>
#include <ecs/component/types/spatial.h>

class HandBehavior : public rvr::Ritual {
public:
    HandBehavior(rvr::type::EntityId id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
    virtual void OnTriggered(rvr::Collider* other) override;

private:
    rvr::Spatial* spatial_;
    float initXScale;
    float initYScale;
    float initZScale;
};
