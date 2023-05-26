#pragma once

#include "ecs/ecs_info.h"
#include "ecs/component/types/ritual.h"
#include "ecs/component/types/spatial.h"
#include "ecs/component/types/audio.h"

class MovingSoundBox : public rvr::Ritual {
public:
    MovingSoundBox(rvr::type::EntityId id);
    virtual void Update(float delta) override;

private:
    rvr::Spatial* spatial_;
    rvr::Audio* audio_;
};
