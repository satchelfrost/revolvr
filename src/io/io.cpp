/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <io/io.h>
#include <io/action/input/bool_action.h>
#include <global_context.h>

namespace rvr {
bool IsPinched (Hand hand) {
    switch (hand) {
        case Hand::Left:
            return GlobalContext::Inst()->GetXrContext()->handTrackerLeft_.IsPinching();
        case Hand::Right:
            return GlobalContext::Inst()->GetXrContext()->handTrackerRight_.IsPinching();
        default:
            THROW("IsPinched expects only Hand::Left or Hand::Right");
    }
}

bool ButtonPressed(ActionType type) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::A:
        case ActionType::B:
            return button->StateTurnedOn(Hand::Right);
        case ActionType::X:
        case ActionType::Y:
            return button->StateTurnedOn(Hand::Left);
        default:
            THROW("ButtonPressed expects only action types A, B, X, or Y")
    }
}

bool ButtonDown(ActionType type) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::A:
        case ActionType::B:
            return button->CurrHandState(Hand::Right).currentState;
        case ActionType::X:
        case ActionType::Y:
            return button->CurrHandState(Hand::Left).currentState;
        default:
            THROW("ButtonDown/Up expects only action types A, B, X, or Y")
    }
}

bool ButtonUp(ActionType type) {
    return !ButtonDown(type);
}

bool ButtonReleased(ActionType type) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::A:
        case ActionType::B:
            return button->StateTurnedOff(Hand::Right);
        case ActionType::X:
        case ActionType::Y:
            return button->StateTurnedOff(Hand::Left);
        default:
            THROW("ButtonReleased expects only action types A, B, X, or Y")
    }
}

bool Touched(ActionType type, Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::ThumbStickTouch:
        case ActionType::ThumbStickRestTouch:
        case ActionType::TriggerTouch:
            return button->CurrHandState(hand).currentState;
        default:
            THROW("Touched/NotTouched expects only action types ThumbRestTouch, ThumbStickTouch, or TriggerTouch")
    }
}

bool NotTouched(ActionType type, Hand hand) {
    return !Touched(type, hand);
}

bool JustTouched(ActionType type, Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::ThumbStickTouch:
        case ActionType::ThumbStickRestTouch:
        case ActionType::TriggerTouch:
            return button->StateTurnedOn(hand);
        default:
            THROW("JustTouched expects only action types ThumbRestTouch, ThumbStickTouch, or TriggerTouch")
    }

}

bool JustUnTouched(ActionType type, Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::ThumbStickTouch:
        case ActionType::ThumbStickRestTouch:
        case ActionType::TriggerTouch:
            return button->StateTurnedOff(hand);
        default:
            THROW("JustUnTouched expects only action types ThumbRestTouch, ThumbStickTouch, or TriggerTouch")
    }
}

bool IsAimPoseActive(Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(ActionType::AimPose);
    auto pose = dynamic_cast<PoseAction*>(action);
    return pose->IsHandActive(hand);
}

bool IsGripPoseActive(Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(ActionType::GripPose);
    auto pose = dynamic_cast<PoseAction*>(action);
    return pose->IsHandActive(hand);
}

XrVector2f GetJoystickXY(Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(ActionType::Joystick);
    auto joystick = dynamic_cast<Joystick*>(action);
    return joystick->GetHandState(hand).currentState;
}

float GetGripTriggerValue(Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(ActionType::GripTrigger);
    auto grip = dynamic_cast<GripTrigger*>(action);
    return grip->GetHandState(hand).currentState;
}

float GetIndexTriggerValue(Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(ActionType::IndexTrigger);
    auto index = dynamic_cast<IndexTrigger*>(action);
    return index->GetHandState(hand).currentState;
}

bool IsThumbStickTouched(Hand hand) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(ActionType::ThumbStickTouch);
    auto thumbstick = dynamic_cast<ThumbStickTouch*>(action);
    return thumbstick->CurrHandState(hand).currentState;
}

void CheckForQuit() {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(ActionType::Menu);
    auto menu = dynamic_cast<BoolAction*>(action);
    XrActionStateBoolean quitValue = menu->CurrHandState(Hand::Left);
    if ((quitValue.isActive == XR_TRUE) && (quitValue.changedSinceLastSync == XR_TRUE) &&
        (quitValue.currentState == XR_TRUE))
        GlobalContext::Inst()->GetXrContext()->RequestExit();
}

void Vibrate(Hand hand, float amplitude, float frequency, XrDuration duration) {
    auto action = GlobalContext::Inst()->GetXrContext()->actionManager.GetAction(ActionType::Vibrate);
    auto vibrate = dynamic_cast<HapticAction*>(action);
    vibrate->ApplyVibration(hand, amplitude, frequency, duration);
}
}