#pragma once

#include "ecs/component/types/ritual.h"

class TimerBoxMaker : public rvr::Ritual {
public:
    TimerBoxMaker(rvr::type::EntityId id);
    virtual void OnTimeout() override;

    rvr::Entity* CreateBoxViaClone() const;
    void AdjustBoxPosition(rvr::Entity* entity);

private:
    int offset_;
    std::vector<rvr::Entity*> spawnedEntities_;
};
