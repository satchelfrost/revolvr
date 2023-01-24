#pragma once

#include <math/xr_linear.h>
#include <ecs/ecs_info.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/ritual.h>
#include <ecs/system/notification/event.h>
#include <ecs/system/notification/collision_event.h>

class SpinningPointer : public rvr::Ritual {
public:
    SpinningPointer(rvr::type::EntityId id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
    virtual void OnNotify(rvr::Event* event) override;
    void RightHandCollided(rvr::CollisionEvent* event);

private:
    rvr::Spatial* spatial_;
    rvr::Collider* collider_;
    int turnAmt_;
    int turnSpeed_;
    int transDirection_;
    const rvr::type::EntityId rightHandColliderId_;
};