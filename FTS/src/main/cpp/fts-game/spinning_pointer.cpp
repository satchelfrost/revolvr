#include "spinning_pointer.h"
#include <ecs/ecs.h>
#include <ecs/system/input_system.h>

SpinningPointer::SpinningPointer(rvr::type::EntityId id) : Ritual(id) {
    spatial_ = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(id);
    turnAmt_ = 0;
    turnSpeed_ = 3;
    transDirection_ = 1;
    wandMoving_ = false;
}

void SpinningPointer::Begin() {
}

void SpinningPointer::Update(float delta) {
    if (rvr::ButtonPressed(rvr::ActionType::A))
        wandMoving_ = !wandMoving_;

    if (wandMoving_)
        MoveWand(delta);
}

void SpinningPointer::OnTriggered(rvr::Collider* other) {}

void SpinningPointer::MoveWand(float delta) {
    // Rotate by the turn speed every frame
    turnAmt_ = (turnAmt_ + turnSpeed_) % 360;
    float angle = 3.14f * (float)turnAmt_ / 180.0f;
    spatial_->pose.orientation = XrQuaternionf_CreateFromVectorAngle({0, 1, 0}, angle);

    // Frame-rate independent translation back and forth
    float transAmt = spatial_->pose.position.z + 0.1f * delta * (float)transDirection_;
    spatial_->pose.position.z = transAmt;
    if (abs(transAmt) > 0.25)
        transDirection_ *= -1;
}
