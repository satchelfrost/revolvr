#include <ecs/system/io_system.h>
#include <action/input/bool_action.h>

namespace rvr::system::io {
bool ButtonPressed(ActionType type) {
    auto action = XrContext::Instance()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::A:
        case ActionType::B:
        case ActionType::X:
        case ActionType::Y:
            return button->StateTurnedOn();
        default:
            THROW("ButtonPressed expects only action types A, B, X, or Y")
    }
}

bool ButtonDown(ActionType type) {
    auto action = XrContext::Instance()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::A:
        case ActionType::B:
        case ActionType::X:
        case ActionType::Y:
            return button->CurrHandState().currentState;
        default:
            THROW("ButtonDown/Up expects only action types A, B, X, or Y")
    }
}

bool ButtonUp(ActionType type) {
    return !ButtonDown(type);
}

bool ButtonReleased(ActionType type) {
    auto action = XrContext::Instance()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::A:
        case ActionType::B:
        case ActionType::X:
        case ActionType::Y:
            return button->StateTurnedOff();
        default:
            THROW("ButtonReleased expects only action types A, B, X, or Y")
    }
}

bool Touched(ActionType type, Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::ThumbStickTouch:
        case ActionType::ThumbStickRestTouch:
        case ActionType::TriggerTouch:
            return button->CurrHandState().currentState;
        default:
            THROW("Touched/NotTouched expects only action types ThumbRestTouch, ThumbStickTouch, or TriggerTouch")
    }
}

bool NotTouched(ActionType type, Hand hand) {
    return !Touched(type, hand);
}

bool JustTouched(ActionType type, Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::ThumbStickTouch:
        case ActionType::ThumbStickRestTouch:
        case ActionType::TriggerTouch:
            return button->StateTurnedOn();
        default:
            THROW("JustTouched expects only action types ThumbRestTouch, ThumbStickTouch, or TriggerTouch")
    }

}

bool JustUnTouched(ActionType type, Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(type);
    auto button = dynamic_cast<BoolAction*>(action);
    switch (type) {
        case ActionType::ThumbStickTouch:
        case ActionType::ThumbStickRestTouch:
        case ActionType::TriggerTouch:
            return button->StateTurnedOff();
        default:
            THROW("JustUnTouched expects only action types ThumbRestTouch, ThumbStickTouch, or TriggerTouch")
    }
}

bool IsAimPoseActive(Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(ActionType::AimPose);
    auto pose = dynamic_cast<PoseAction*>(action);
    return pose->IsHandActive(hand);
}

bool IsGripPoseActive(Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(ActionType::GripPose);
    auto pose = dynamic_cast<PoseAction*>(action);
    return pose->IsHandActive(hand);
}

XrVector2f GetJoystickXY(Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(ActionType::Joystick);
    auto joystick = dynamic_cast<Joystick*>(action);
    return joystick->GetHandState(hand).currentState;
}

float GetGripTriggerValue(Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(ActionType::GripTrigger);
    auto grip = dynamic_cast<GripTrigger*>(action);
    return grip->GetHandState(hand).currentState;
}

float GetIndexTriggerValue(Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(ActionType::IndexTrigger);
    auto index = dynamic_cast<IndexTrigger*>(action);
    return index->GetHandState(hand).currentState;
}

bool IsThumbStickTouched(Hand hand) {
    auto action = XrContext::Instance()->actionManager.GetAction(ActionType::ThumbStickTouch);
    auto thumbstick = dynamic_cast<ThumbStickTouch*>(action);
    return thumbstick->CurrHandState().currentState;
}

void CheckForQuit() {
    auto action = XrContext::Instance()->actionManager.GetAction(ActionType::Menu);
    auto menu = dynamic_cast<BoolAction*>(action);
    XrActionStateBoolean quitValue = menu->CurrHandState();
    auto session = XrContext::Instance()->session;
    if ((quitValue.isActive == XR_TRUE) && (quitValue.changedSinceLastSync == XR_TRUE) &&
        (quitValue.currentState == XR_TRUE))
        CHECK_XRCMD(xrRequestExitSession(session));
}

void Vibrate(Hand hand, float amplitude, float frequency, XrDuration duration) {
    auto action = XrContext::Instance()->actionManager.GetAction(ActionType::Vibrate);
    auto vibrate = dynamic_cast<HapticAction*>(action);
    auto session = XrContext::Instance()->session;
    vibrate->ApplyVibration(session, hand, amplitude, frequency, duration);
}
}