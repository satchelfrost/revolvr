#include <movement_ritual.h>
#include <global_context.h>
#include <helper_macros.h>
#include "io/io.h"
#include <cmath>

#define PLAYER_ID 1
#define JOYSTICK_PIVOT_ID 12

MovementRitual::MovementRitual(rvr::type::EntityId id) : Ritual(id) {
    playerSpatial_ = GetComponent<rvr::Spatial>(PLAYER_ID);
    joystickPivotPointSpatial_ = GetComponent<rvr::Spatial>(JOYSTICK_PIVOT_ID);
}

void MovementRitual::Begin() {}

void MovementRitual::Update(float delta) {
    auto rightJoy = GetJoystickXY(rvr::Hand::Right);

    // forward/backward movement with right joystick y-axis
    auto position = playerSpatial_->GetLocal().GetPosition();
    auto forward = -playerSpatial_->GetLocal().GetZAxis();
    position += forward * rightJoy.y * delta;
    playerSpatial_->SetLocalPosition(position);

    // Rotate left/right movement with right joystick x-axis
    float angle = -rightJoy.x * delta;
    auto transform = playerSpatial_->GetLocal().Rotated({0, 1, 0}, angle);
    playerSpatial_->SetLocal(transform);

    // Visualize the joystick
    auto xAxis = joystickPivotPointSpatial_->GetLocal().GetXAxis();
    auto yAxis = joystickPivotPointSpatial_->GetLocal().GetYAxis();
    float forwardBackwardTilt = -rightJoy.y * 45;
    float leftRightTilt = -rightJoy.x * 45;
//    auto orientation = rvr::math::quaternion::FromAxisAngle(xAxis, forwardBackwardTilt);
//    orientation *= rvr::math::quaternion::FromAxisAngle(yAxis, leftRightTilt);
    glm::quat tiltQuaternion = glm::angleAxis(glm::radians(forwardBackwardTilt), xAxis)
                               * glm::angleAxis(glm::radians(leftRightTilt), yAxis);
//    joystickPivotPointSpatial_->SetLocalOrientation(orientation);
    joystickPivotPointSpatial_->SetLocalOrientation(tiltQuaternion);
}