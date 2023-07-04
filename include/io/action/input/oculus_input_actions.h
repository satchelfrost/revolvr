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

#include <io/action/input/float_action.h>
#include <io/action/input/pose_action.h>
#include <io/action/input/bool_action.h>
#include <io/action/input/vec2_action.h>

namespace rvr {
class GripTrigger : public FloatAction {
public:
    GripTrigger(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class IndexTrigger : public FloatAction {
public:
    IndexTrigger(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class GripPose : public PoseAction {
public:
    GripPose(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class Menu : public BoolAction {
public:
    Menu(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class A : public BoolAction {
public:
    A(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class B : public BoolAction {
public:
    B(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class X : public BoolAction {
public:
    X(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class Y : public BoolAction {
public:
    Y(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class AimPose : public PoseAction {
public:
    AimPose(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class ThumbStickTouch : public BoolAction {
public:
    ThumbStickTouch(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class ThumbStickRestTouch : public BoolAction {
public:
    ThumbStickRestTouch(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class TriggerTouch : public BoolAction {
public:
    TriggerTouch(XrActionSet actionSet, HandPathArray handSubactionPath);
};

class Joystick : public Vec2Action {
public:
    Joystick(XrActionSet actionSet, HandPathArray handSubactionPath);
};
}
