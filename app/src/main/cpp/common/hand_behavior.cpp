#include "common/hand_behavior.h"
#include "common.h"
#include "ecs/ecs.h"
#include "ecs/component/types/spatial.h"
#include "action/io.h"
#include "global_context.h"
#include "common/helper_macros.h"

HandBehavior::HandBehavior(rvr::type::EntityId id) : Ritual(id) {
    spatial_ = GetComponent<rvr::Spatial>(id);
    initScale_ = spatial_->GetLocal().GetScale();
}

void HandBehavior::Begin() {}

void HandBehavior::Update(float delta) {
    // Id 1 is the left hand from the rvr file
    rvr::Hand hand = (id == 1) ? rvr::Hand::Left : rvr::Hand::Right;

    float value = GetGripTriggerValue(hand);
    if (value > 0.9)
        Vibrate(hand, 0.5);
    float multiplier = 1.0f - 0.5f * value;
    glm::vec3 newScale = initScale_ * multiplier;
    spatial_->SetLocalScale(newScale);
}

void HandBehavior::OnTriggered(rvr::Collider* other) {
    // Check for dummy collision i.e. hands are not tracked yet and so they will be at the origin
    auto hand1 = GetComponent<rvr::Spatial>(id);
    auto hand2 = GetComponent<rvr::Spatial>(other->id);
    auto hand1Position = hand1->GetWorld().GetPosition();
    auto hand2Position = hand2->GetWorld().GetPosition();

    if (hand1Position == glm::zero<glm::vec3>() ||
        hand2Position == glm::zero<glm::vec3>())
        return;

    Log::Write(Log::Level::Info, Fmt("Collision source %d", id));
    Log::Write(Log::Level::Info, Fmt("Collision other %d", other->id));
}
