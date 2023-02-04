#pragma once

#include <math/xr_linear.h>
#include <ecs/ecs_info.h>
#include <ecs/component/types/ritual.h>

class LeftHandBehavior : public rvr::Ritual {
public:
    LeftHandBehavior(rvr::type::EntityId id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
    virtual void OnTriggered(rvr::Collider* other) override;
    void RightHandCollided(rvr::Collider* other);

private:
};
