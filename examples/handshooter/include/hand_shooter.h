#pragma once

#include <ecs/component/types/ritual.h>
#include <ecs/component/types/spatial.h>
#include <random>

class HandShooter : public rvr::Ritual {
public:
    HandShooter(rvr::type::EntityId id);
    
    virtual void Begin() override;
    virtual void Update(float delta) override;
    virtual void OnTriggered(rvr::Collider* other) override;
    virtual void OnTimeout() override;
private:
    void Reset();
    void SetVisibilityOfBoxes(bool visibility);
    rvr::Spatial* shooterBoxSpatial_;
    bool isDetached_;
    float projectileSpeed_;
    rvr::math::Transform originalTransform_;
    bool breakNoisePlayed_;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<int> distribution_;
};
