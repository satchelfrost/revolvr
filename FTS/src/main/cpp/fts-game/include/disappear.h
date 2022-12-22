#pragma once

#include <ecs/ecs_info.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/ritual.h>

class Disappear : public rvr::Ritual {
public:
    Disappear(rvr::type::EntityId id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
    virtual void OnOverlap(rvr::Collider* collider) override ;
};
