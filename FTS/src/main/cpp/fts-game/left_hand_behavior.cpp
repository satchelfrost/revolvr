#include "left_hand_behavior.h"
#include <common.h>
#include <ecs/ecs.h>
#include <ecs/component/types/spatial.h>

LeftHandBehavior::LeftHandBehavior(rvr::type::EntityId id) : Ritual(id) {}

void LeftHandBehavior::Begin() {}

void LeftHandBehavior::Update(float delta) {}

void LeftHandBehavior::OnTriggered(rvr::Collider* other) {
    RightHandCollided(other);
}

void LeftHandBehavior::RightHandCollided(rvr::Collider* other) {
    // Check for dummy collision i.e. hands are not tracked yet and so they will be at the origin
    auto hand1 = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(id);
    auto hand2 = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(other->id);
    if ((hand1->worldPose.position.x == 0 &&
         hand1->worldPose.position.y == 0 &&
         hand1->worldPose.position.z == 0) ||
        (hand2->worldPose.position.x == 0 &&
         hand2->worldPose.position.y == 0 &&
         hand2->worldPose.position.z == 0))
        return;

    Log::Write(Log::Level::Info, Fmt("Collision source %d", id));
    Log::Write(Log::Level::Info, Fmt("Collision other %d", other->id));
}
