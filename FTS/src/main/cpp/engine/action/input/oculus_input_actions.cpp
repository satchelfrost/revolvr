#include <action/input/oculus_input_actions.h>

namespace rvr {
GripTrigger::GripTrigger(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath) :
FloatAction(actionSet, handSubactionPath, "/input/squeeze/value", ActionType::GripTrigger) {}

IndexTrigger::IndexTrigger(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
FloatAction(actionSet, handSubactionPath, "/input/trigger/value", ActionType::IndexTrigger) {}

GripPose::GripPose(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
PoseAction(actionSet, handSubactionPath, "/input/grip/pose", ActionType::GripPose) {}

Menu::Menu(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
BoolAction(actionSet, handSubactionPath, "/input/menu/click", ActionType::Menu, Hand::Left){}

A::A(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
BoolAction(actionSet, handSubactionPath, "/input/a/click", ActionType::A, Hand::Right){}

B::B(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
BoolAction(actionSet, handSubactionPath, "/input/b/click", ActionType::B, Hand::Right){}

X::X(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
BoolAction(actionSet, handSubactionPath, "/input/x/click", ActionType::X, Hand::Left){}

Y::Y(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
BoolAction(actionSet, handSubactionPath, "/input/y/click", ActionType::Y, Hand::Left){}

AimPose::AimPose(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
PoseAction(actionSet, handSubactionPath, "/input/aim/pose", ActionType::AimPose) {}

ThumbStickTouch::ThumbStickTouch(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
BoolAction(actionSet, handSubactionPath, "/input/thumbstick/touch",
           ActionType::ThumbStickTouch, Hand::Both) {}

ThumbStickRestTouch::ThumbStickRestTouch(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
BoolAction(actionSet, handSubactionPath, "/input/thumbrest/touch",
           ActionType::ThumbStickRestTouch, Hand::Both) {}

TriggerTouch::TriggerTouch(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
BoolAction(actionSet, handSubactionPath, "/input/trigger/touch",
           ActionType::TriggerTouch, Hand::Both) {}

Joystick::Joystick(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
Vec2Action(actionSet, handSubactionPath, "/input/thumbstick", ActionType::Joystick) {}
}