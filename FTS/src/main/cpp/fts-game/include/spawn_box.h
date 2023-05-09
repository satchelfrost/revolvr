#pragma once

#include <ecs/ecs_info.h>
#include <ecs/component/types/ritual.h>

class SpawnBox : public rvr::Ritual {
public:
    SpawnBox(rvr::type::EntityId id);
    virtual void Update(float delta) override;

private:
    int offset_;
    void ManualApproach();
    void CloneApproach();
    static rvr::Entity* CreateBoxManually();
    static rvr::Entity* CreateBoxViaClone();
    void AdjustBoxPosition(rvr::Entity* entity);

    std::vector<rvr::Entity*> spawnedEntities_;

    static constexpr int boxToClonesId = 6;
};
