#pragma once

#include <ecs/component/types/ritual.h>

class HandTrackingRitual : public rvr::Ritual {
public:
    HandTrackingRitual(rvr::type::EntityId id);
    
    virtual void Update(float delta) override;
    virtual void OnTriggered(rvr::Collider* other) override;

};
