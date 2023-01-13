#include <input_state.h>
#include <common.h>

namespace rvr {
InputState::InputState(XrSession session) : session_(session) {
    handScale = {{1.0f, 1.0f}};

    actionNameMap.emplace(ActionName::LeftGrab, std::make_pair(grabAction, handSubactionPath[Side::LEFT]));
//    XrAction grabAction{XR_NULL_HANDLE};
//    XrAction poseAction{XR_NULL_HANDLE};
//    XrAction vibrateAction{XR_NULL_HANDLE};
//    XrAction quitAction{XR_NULL_HANDLE};
}

void InputState::SyncActions(XrSession& session) {
    const XrActiveActionSet activeActionSet{actionSet, XR_NULL_PATH};
    XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    CHECK_XRCMD(xrSyncActions(session, &syncInfo));
}

XrActionStateFloat InputState::GetActionStateFloat(XrAction action, XrPath subactionPath, XrSession& session) {
    XrActionStateGetInfo getInfo = {};
    getInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
    getInfo.action = action;
    getInfo.subactionPath = subactionPath;
    XrActionStateFloat state = {};
    state.type = XR_TYPE_ACTION_STATE_FLOAT;
    CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &state));
    return state;
}

void InputState::VibrateWithAmplitude(float amplitude, XrSession& session, int hand) {
    XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
    vibration.amplitude = amplitude;
    vibration.duration = XR_MIN_HAPTIC_DURATION;
    vibration.frequency = XR_FREQUENCY_UNSPECIFIED;

    XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
    hapticActionInfo.action = vibrateAction;
    hapticActionInfo.subactionPath = handSubactionPath[hand];
    CHECK_XRCMD(xrApplyHapticFeedback(session, &hapticActionInfo, (XrHapticBaseHeader*)&vibration));

}

void InputState::UpdateActionStateFloat(XrSession& session, ActionName actionName) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = actionNameMap.at(actionName).first;
    getInfo.subactionPath = actionNameMap.at(actionName).second;

    XrActionStateFloat grabValue{XR_TYPE_ACTION_STATE_FLOAT};
    CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &grabValue));

}
}