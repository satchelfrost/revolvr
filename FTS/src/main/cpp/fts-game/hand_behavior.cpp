#include "hand_behavior.h"
#include <common.h>
#include <ecs/ecs.h>
#include <ecs/component/types/spatial.h>
#include <ecs/system/io_system.h>

HandBehavior::HandBehavior(rvr::type::EntityId id) : Ritual(id) {
    spatial_ = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(id);
    initXScale = spatial_->local.scale.x;
    initYScale = spatial_->local.scale.y;
    initZScale = spatial_->local.scale.z;
}

void HandBehavior::Begin() {}

void HandBehavior::Update(float delta) {
    // Id 1 is the left hand from the rvr file
    rvr::Hand hand = (id == 1) ? rvr::Hand::Left : rvr::Hand::Right;

    float value = rvr::system::io::GetGripTriggerValue(hand);
    if (value > 0.9)
        rvr::system::io::Vibrate(hand, 0.5);
    float multiplier = 1.0f - 0.5f * value;
    spatial_->local.scale.x = initXScale * multiplier;
    spatial_->local.scale.y = initYScale * multiplier;
    spatial_->local.scale.z = initZScale * multiplier;
}

void HandBehavior::OnTriggered(rvr::Collider* other) {
    // Check for dummy collision i.e. hands are not tracked yet and so they will be at the origin
    auto hand1 = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(id);
    auto hand2 = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(other->id);
    if ((hand1->world.pose.position.x == 0 &&
         hand1->world.pose.position.y == 0 &&
         hand1->world.pose.position.z == 0) ||
        (hand2->world.pose.position.x == 0 &&
         hand2->world.pose.position.y == 0 &&
         hand2->world.pose.position.z == 0))
        return;

    Log::Write(Log::Level::Info, Fmt("Collision source %d", id));
    Log::Write(Log::Level::Info, Fmt("Collision other %d", other->id));
}
