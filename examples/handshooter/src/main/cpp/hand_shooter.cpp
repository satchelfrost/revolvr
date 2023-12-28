#include <hand_shooter.h>
#include <global_context.h>
#include <helper_macros.h>
#include "io/io.h"
#include <ecs/component/types/timer.h>
#include <ecs/component/types/mesh.h>

HandShooter::HandShooter(rvr::type::EntityId id) : Ritual(id), gen_(rd_()), distribution_(-10, 10){
    shooterBoxSpatial_ = GetComponent<rvr::Spatial>(id);
    originalTransform_ = shooterBoxSpatial_->GetLocal();
    isDetached_ = false;
    projectileSpeed_ = 10.0f;
    breakNoisePlayed_ = false;
}

void HandShooter::Begin() {}

void HandShooter::Update(float delta) {
    if (IsPinched(rvr::Hand::Right) && !isDetached_) {
        // Play shoot sound...
        auto shootSound = GetComponent<rvr::Audio>(id);
        if (shootSound)
            shootSound->Play();

        // Reparent from hand to the origin
        rvr::Entity* shooterBox = GetEntity(id);
        rvr::Entity* origin = GetEntity(0);

        shooterBoxSpatial_->SetLocal(shooterBoxSpatial_->GetWorld());
        origin->AddChild(shooterBox);
        isDetached_ = true;

        // Start positional and visual reset timer
        auto timer = GetComponent<rvr::Timer>(id);
        timer->Start();
    }

    if (isDetached_) {
        auto position = shooterBoxSpatial_->GetLocal().GetPosition();
        auto forward = -shooterBoxSpatial_->GetLocal().GetYAxis();
        position += forward * projectileSpeed_ * delta;
        shooterBoxSpatial_->SetLocalPosition(position);
    }
}

void HandShooter::OnTriggered(rvr::Collider* other) {
    auto breakNoise = GetComponent<rvr::Audio>(other->id);
    if (breakNoise) {
        if (!breakNoisePlayed_) {
            breakNoise->Play();
            breakNoisePlayed_ = true;
        }
    }

    SetVisibilityOfBoxes(false);
    Log::Write(Log::Level::Info,"Collision has occurred");
}

void HandShooter::OnTimeout() {
    if (isDetached_)
        Reset();
    Log::Write(Log::Level::Info,"Timer has reset");
}

void HandShooter::Reset() {
    //Reset projectile by attaching to hand
    rvr::Entity* shooterBox = GetEntity(id);
    int RIGHT_CENTER_HAND_JOINT = 34;
    rvr::Entity* rightHand = GetEntity(RIGHT_CENTER_HAND_JOINT);

    shooterBoxSpatial_->SetLocal(originalTransform_);
    rightHand->AddChild(shooterBox);
    isDetached_ = false;
    SetVisibilityOfBoxes(true);

    // Set collided with box with new random position
    int collidedWithBox = 55;
    auto otherBox = GetComponent<rvr::Spatial>(collidedWithBox);
    if (otherBox) {
        auto position = otherBox->GetLocal().GetPosition();
        position.x = (float)distribution_(gen_);
        position.z = (float)distribution_(gen_);
        // Avoid spawning box too close to player
        if (abs(position.x) < 3)
            position.x = 3;
        if (abs(position.z) < 3)
            position.z = 3;
        otherBox->SetLocalPosition(position);
    }

    // Allow break noise again
    breakNoisePlayed_ = false;
}

void HandShooter::SetVisibilityOfBoxes(bool visibility) {
    constexpr int collidedWithBox = 55;
    auto otherMesh = GetComponent<rvr::Mesh>(collidedWithBox);
    otherMesh->SetVisibilityRecursive(visibility);
    auto thisMesh = GetComponent<rvr::Mesh>(id);
    thisMesh->SetVisibilityRecursive(visibility);
}

