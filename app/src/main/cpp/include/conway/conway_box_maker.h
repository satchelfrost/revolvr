#pragma once

#include "ecs/component/types/ritual.h"
#include "conway_engine.h"

class ConwayBoxMaker : public rvr::Ritual {
public:
    ConwayBoxMaker(rvr::type::EntityId id);
    virtual void OnTimeout() override;
    rvr::Entity* CreateBoxViaClone() const;

private:
    void AdjustBoxPosition(rvr::Entity* entity, int yOffset, int xOffset) const;

    rvr::type::EntityId cloneBoxEntityId_;
    ConwayEngine conwayEngine_;
    std::vector<rvr::Entity*> entities_;
    float boxHalfExtent_;
};
