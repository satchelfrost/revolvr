#include <action/output/vibrate.h>
#include <common.h>

namespace rvr {
Vibrate::Vibrate(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath) :
Action(actionSet, handSubactionPath, "/output/haptic", ActionType::Vibrate, Hand::Both) {
    CreateAction(XR_ACTION_TYPE_VIBRATION_OUTPUT);
    Reset();
}

void Vibrate::Update(XrSession& session) {
    Reset();
}

void Vibrate::Reset() {
    vibration_.amplitude = 0.0;
    vibration_.duration = XR_MIN_HAPTIC_DURATION;
    vibration_.frequency = XR_FREQUENCY_UNSPECIFIED;
}

void Vibrate::ApplyVibration(XrSession &session, Hand hand, float amplitude, float frequency, XrDuration duration) {
    XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
    hapticActionInfo.action = action_;
    hapticActionInfo.subactionPath = handSubactionPath_[(int)hand];
    vibration_.amplitude = amplitude;
    vibration_.frequency = frequency;
    vibration_.duration  = duration;
    CHECK_XRCMD(xrApplyHapticFeedback(session, &hapticActionInfo, (XrHapticBaseHeader*)&vibration_));
}
}
