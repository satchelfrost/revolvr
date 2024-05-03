#pragma once

#include <ecs/component/types/ritual.h>
#include <ecs/component/types/spatial.h>

class Movement : public rvr::Ritual {
public:
    Movement(rvr::type::EntityId id);
    
    virtual void Begin() override;
    virtual void Update(float delta) override;
    virtual void OnTriggered(rvr::Collider* other) override;
    virtual void OnTimeout() override;

    rvr::Spatial* arenaOrigin_;
    rvr::Spatial* head_;
};
