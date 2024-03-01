#include <movement.h>
#include <global_context.h>
#include <helper_macros.h>
#include <io/io.h>
#include <stdio.h>

#define MOVE_SPEED 25.0f
#define SCALE_SPEED 1.0f
#define MIN_SCALE 0.01f
#define MAX_SCALE 1.0f

Movement::Movement(rvr::type::EntityId id) : Ritual(id) {
    arenaOrigin_ = GetComponent<rvr::Spatial>(id);
}

void Movement::Begin() {}

void Movement::Update(float delta) {

    auto rightJoy = GetJoystickXY(rvr::Hand::Right);
    if (GetGripTriggerValue(rvr::Hand::Right) != 0.0) {
        /* Right joystick + grip ->  rotation about vertical axis */
        float angle = rightJoy.x * delta;
        auto transform = arenaOrigin_->GetLocal().Rotated({0, 1, 0}, angle);
        arenaOrigin_->SetLocal(transform);
    } else {
        /* Right joystick -> left/right/forward/backward */
        auto pos = arenaOrigin_->GetLocal().GetPosition();
        auto scale = arenaOrigin_->GetLocal().GetScale();
        pos.z -= rightJoy.y * delta * MOVE_SPEED * scale.x;
        pos.x += rightJoy.x * delta * MOVE_SPEED * scale.x;
        arenaOrigin_->SetLocalPosition(pos);
    }

    auto leftJoy = GetJoystickXY(rvr::Hand::Left);
    if (GetGripTriggerValue(rvr::Hand::Left) != 0.0) {
        /* Left joystick + grip -> up/down */
        auto pos = arenaOrigin_->GetLocal().GetPosition();
        auto scale = arenaOrigin_->GetLocal().GetScale();
        pos.y += leftJoy.y * delta * MOVE_SPEED / 2.0f * scale.x;
        arenaOrigin_->SetLocalPosition(pos);
    } else {
        /* Left joystick -> scale */
        auto scale = arenaOrigin_->GetLocal().GetScale();
        float scaleAmt = leftJoy.y * delta * SCALE_SPEED * scale.x;
        if (scale.x + scaleAmt < MIN_SCALE) {
            scale = {MIN_SCALE, MIN_SCALE, MIN_SCALE};
        }
        else if (scale.x + scaleAmt > MAX_SCALE) {
            scale = {MAX_SCALE, MAX_SCALE, MAX_SCALE};
        }
        else {
            scale.x += scaleAmt;
            scale.y += scaleAmt;
            scale.z += scaleAmt;
        }
        arenaOrigin_->SetLocalScale(scale);
    }
}

void Movement::OnTriggered(rvr::Collider* other) {}

void Movement::OnTimeout() {}

