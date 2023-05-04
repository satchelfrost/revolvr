#pragma once

#include <ecs/ecs_info.h>
#include <ecs/component/types/ritual.h>

class SpawnBox : public rvr::Ritual {
public:
    SpawnBox(rvr::type::EntityId id);
    
    virtual void Begin() override;
    virtual void Update(float delta) override;
    virtual void OnTriggered(rvr::Collider* other) override;

private:
    int offset_;
    rvr::Entity* CreateBoxManually();
    static rvr::Entity* CreateBoxViaClone();
    std::vector<rvr::Entity*> spawnedEntities_;
};
