#include <movement.h>
#include <global_context.h>
#include <helper_macros.h>
#include <io/io.h>
#include <stdio.h>

#define MOVE_SPEED 3.0f
#define SCALE_SPEED 0.1f
#define MIN_SCALE 0.01f
#define MAX_SCALE 1.0f

Movement::Movement(rvr::type::EntityId id) : Ritual(id) {
    arenaOrigin_ = GetComponent<rvr::Spatial>(id);
}

void Movement::Begin() {}

void Movement::Update(float delta) {

    auto rightJoy = GetJoystickXY(rvr::Hand::Right);
    if (GetGripTriggerValue(rvr::Hand::Right) != 0.0) {
        float angle = -rightJoy.x * delta;
        auto transform = arenaOrigin_->GetLocal().Rotated({0, 1, 0}, angle);
        arenaOrigin_->SetLocal(transform);
    }
    else if (GetIndexTriggerValue(rvr::Hand::Right) != 0.0) {
        auto pos = arenaOrigin_->GetLocal().GetPosition();
        pos.y += rightJoy.y * delta * MOVE_SPEED / 2.0f;
        arenaOrigin_->SetLocalPosition(pos);
    }
    else {
        auto pos = arenaOrigin_->GetLocal().GetPosition();
        pos.z -= rightJoy.y * delta * MOVE_SPEED;
        pos.x += rightJoy.x * delta * MOVE_SPEED;
        arenaOrigin_->SetLocalPosition(pos);
    }

    // Left joystick handles model scale
    auto leftJoy = GetJoystickXY(rvr::Hand::Left);
    auto scale = arenaOrigin_->GetLocal().GetScale();
    float scaleAmt = leftJoy.y * delta * SCALE_SPEED;
    if (scale.x * (1.0f - scaleAmt) < MIN_SCALE) {
        scale = {MIN_SCALE, MIN_SCALE, MIN_SCALE};
    }
    else if (scale.x * (1.0f - scaleAmt) > MAX_SCALE) {
        scale = {MAX_SCALE, MAX_SCALE, MAX_SCALE};
    }
    else {
        scale.x += (scaleAmt);
        scale.y += (scaleAmt);
        scale.z += (scaleAmt);
    }
    arenaOrigin_->SetLocalScale(scale);
}

void Movement::OnTriggered(rvr::Collider* other) {}

void Movement::OnTimeout() {}

