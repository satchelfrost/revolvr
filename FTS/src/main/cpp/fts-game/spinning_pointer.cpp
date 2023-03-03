#include "spinning_pointer.h"
#include <ecs/ecs.h>
#include <ecs/system/io_system.h>
#include <include/math/linear_math.h>

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
    if (rvr::system::io::ButtonPressed(rvr::ActionType::A))
        wandMoving_ = !wandMoving_;

    if (wandMoving_)
        MoveWand(delta);
}

void SpinningPointer::OnTriggered(rvr::Collider* other) {}

void SpinningPointer::MoveWand(float delta) {
    // Rotate by the turn speed every frame
    turnAmt_ = (turnAmt_ + turnSpeed_) % 360;
    auto angle = (float)turnAmt_;
    auto axis = spatial_->GetLocal().GetYAxis();
    auto orientation = rvr::math::quaternion::FromAxisAngle(axis, angle);
    spatial_->SetLocalOrientation(orientation);

    // Frame-rate independent translation back and forth
    glm::vec3 velocity{0, 0, 0.1};
    glm::vec3 offset = velocity * (delta * (float)transDirection_);
    glm::vec3 newPosition = spatial_->GetLocal().GetPosition() + offset;
    spatial_->SetLocalPosition(newPosition);
    if (abs(newPosition.z) > 0.25)
        transDirection_ *= -1;
}
