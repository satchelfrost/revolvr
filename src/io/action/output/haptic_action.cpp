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

#include <action/output/haptic_action.h>
#include <common.h>

#include <utility>

namespace rvr {
HapticAction::HapticAction(XrActionSet actionSet, HandPathArray handSubactionPath,
                           std::string actionPath, ActionType type) :
Action(actionSet, handSubactionPath, std::move(actionPath), type, Hand::Both) {
    CreateAction(XR_ACTION_TYPE_VIBRATION_OUTPUT);
    Reset();
}

void HapticAction::Update(XrSession& session) {
    Reset();
}

void HapticAction::Reset() {
    vibration_.amplitude = 0.0;
    vibration_.duration = XR_MIN_HAPTIC_DURATION;
    vibration_.frequency = XR_FREQUENCY_UNSPECIFIED;
}

void HapticAction::ApplyVibration(XrSession &session, Hand hand, float amplitude, float frequency, XrDuration duration) {
    XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
    hapticActionInfo.action = action_;
    hapticActionInfo.subactionPath = GetSubactionPath(hand);
    vibration_.amplitude = amplitude;
    vibration_.frequency = frequency;
    vibration_.duration  = duration;
    CHECK_XRCMD(xrApplyHapticFeedback(session, &hapticActionInfo, (XrHapticBaseHeader*)&vibration_));
}
}
