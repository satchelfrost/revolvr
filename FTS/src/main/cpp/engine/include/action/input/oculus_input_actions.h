#pragma once

#include <action/input/float_action.h>
#include <action/input/pose_action.h>
#include <action/input/bool_action.h>
#include <action/input/vec2_action.h>

namespace rvr {
class GripTrigger : public FloatAction {
public:
    GripTrigger(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class IndexTrigger : public FloatAction {
public:
    IndexTrigger(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class GripPose : public PoseAction {
public:
    GripPose(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class Menu : public BoolAction {
public:
    Menu(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class A : public BoolAction {
public:
    A(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class B : public BoolAction {
public:
    B(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class X : public BoolAction {
public:
    X(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class Y : public BoolAction {
public:
    Y(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class AimPose : public PoseAction {
public:
    AimPose(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class ThumbStickTouch : public BoolAction {
public:
    ThumbStickTouch(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class ThumbStickRestTouch : public BoolAction {
public:
    ThumbStickRestTouch(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class TriggerTouch : public BoolAction {
public:
    TriggerTouch(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};

class Joystick : public Vec2Action {
public:
    Joystick(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};
}
