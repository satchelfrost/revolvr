#include "spinning_pointer.h"
#include <ecs/ecs.h>

SpinningPointer::SpinningPointer(rvr::type::EntityId id) : Ritual(id), rightHandColliderId_(15) {
    spatial_ = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(id);
    spatial_->scale = {0.01, 0.01, 0.5};
    turnAmt_ = 0;
    turnSpeed_ = 3;
    transDirection_ = 1;

    Log::Write(Log::Level::Info, "Adding observer from RightHandCollider");
    collider_ = rvr::ECS::Instance()->GetComponent<rvr::Collider>(rightHandColliderId_);
    collider_->AddObserver(this);
}

void SpinningPointer::Begin() {
}

void SpinningPointer::Update(float delta) {
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

void SpinningPointer::OnNotify(rvr::Event* event) {
    switch (event->type) {
        case rvr::EventType::Collision:
            RightHandCollided(reinterpret_cast<rvr::CollisionEvent*>(event));
            break;
    }
}

void SpinningPointer::RightHandCollided(rvr::CollisionEvent *event) {
    // Check for dummy collision i.e. hands are not tracked yet and so they will be at the origin
    auto hand1 = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(event->src->id);
    auto hand2 = rvr::ECS::Instance()->GetComponent<rvr::Spatial>(event->other->id);
    if ((hand1->worldPose.position.x == 0 &&
         hand1->worldPose.position.y == 0 &&
         hand1->worldPose.position.z == 0) ||
        (hand2->worldPose.position.x == 0 &&
         hand2->worldPose.position.y == 0 &&
         hand2->worldPose.position.z == 0))
        return;

    Log::Write(Log::Level::Info, Fmt("Collision source %d", event->src->id));
    Log::Write(Log::Level::Info, Fmt("Collision other %d", event->other->id));
    if (event->src->id == rightHandColliderId_ && !canUpdate) {
        Log::Write(Log::Level::Info, "Removing observer from RightHandCollider");
        collider_->RemoveObserver(this);
        canUpdate = true;
    }
}
