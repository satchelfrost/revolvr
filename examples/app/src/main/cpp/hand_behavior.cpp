#include "hand_behavior.h"
#include "common.h"
#include "ecs/ecs.h"
#include "ecs/component/types/spatial.h"
#include "ecs/component/types/mesh.h"
#include "io/io.h"
#include "global_context.h"
#include "helper_macros.h"

HandBehavior::HandBehavior(rvr::type::EntityId id) : Ritual(id) {
    spatial_ = GetComponent<rvr::Spatial>(id);
    initScale_ = spatial_->GetLocal().GetScale();
}

void HandBehavior::Begin() {}

void HandBehavior::Update(float delta) {
    // Id 1 is the left hand from the rvr file
    rvr::Hand hand = (id == 2) ? rvr::Hand::Left : rvr::Hand::Right;

    float value = GetGripTriggerValue(hand);
    if (value > 0.9)
        Vibrate(hand, 0.5);
    float multiplier = 1.0f - 0.5f * value;
    glm::vec3 newScale = initScale_ * multiplier;
    spatial_->SetLocalScale(newScale);

    if (ButtonPressed(rvr::ActionType::A)) {
        auto mesh = GetComponent<rvr::Mesh>(id);
        mesh->SetVisibilityRecursive(true);
    }
}

void HandBehavior::OnTriggered(rvr::Collider* other) {
    auto mesh = GetComponent<rvr::Mesh>(other->id);
    mesh->SetVisibilityRecursive(false);
}
