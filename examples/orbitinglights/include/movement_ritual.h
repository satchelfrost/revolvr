#pragma once

#include <ecs/component/types/ritual.h>
#include <ecs/component/types/spatial.h>

class MovementRitual : public rvr::Ritual {
public:
    MovementRitual(rvr::type::EntityId id);
    
    virtual void Begin() override;
    virtual void Update(float delta) override;

private:
    rvr::Spatial* playerSpatial_;
    rvr::Spatial* joystickPivotPointSpatial_;
};
