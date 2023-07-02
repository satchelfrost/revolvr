/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include "action.h"
#include "../xr_context.h"

namespace rvr {
// Hand tracking
bool IsPinched(Hand hand);

// Buttons
bool ButtonPressed(ActionType type);
bool ButtonDown(ActionType type);
bool ButtonUp(ActionType type);
bool ButtonReleased(ActionType type);

// Touch
bool Touched(ActionType type, Hand hand);
bool UnTouched(ActionType type, Hand hand);
bool JustTouched(ActionType type, Hand hand);
bool JustUnTouched(ActionType type, Hand hand);

// Pose
bool IsAimPoseActive(Hand hand);
bool IsGripPoseActive(Hand hand);

// Joystick
XrVector2f GetJoystickXY(Hand hand);

// Triggers
float GetGripTriggerValue(Hand hand);
float GetIndexTriggerValue(Hand hand);

// Quit
void CheckForQuit();

// Vibration
void Vibrate(Hand hand, float amplitude = 0.0, float frequency = 0.0, XrDuration duration = XR_MIN_HAPTIC_DURATION);
}