#include <input_state.h>
#include <common.h>

namespace rvr {
InputState::InputState() {
    handScale = {{1.0f, 1.0f}};
}

void InputState::Init(XrInstance instance) {
    // Get the XrPath for the left and right hands - we will use them as subaction paths.
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left", &handSubactionPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right", &handSubactionPath[Side::RIGHT]));
    CreateActionSet(instance);
    CreateGrabActions();
    CreatePoseActions();
    CreateVibrateActions();
    CreateQuitAction();
}

void InputState::CreateActionSet(XrInstance instance) {
    XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
    strcpy_s(actionSetInfo.actionSetName, "gameplay");
    strcpy_s(actionSetInfo.localizedActionSetName, "Gameplay");
    actionSetInfo.priority = 0;
    CHECK_XRCMD(xrCreateActionSet(instance, &actionSetInfo, &actionSet));
}

void InputState::CreateGrabActions() {
    // First create the actions
    XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
    actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
    strcpy_s(actionInfo.actionName, "grab_object");
    strcpy_s(actionInfo.localizedActionName, "Grab Object");
    actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
    actionInfo.subactionPaths = handSubactionPath.data();
    CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &grabAction));

    // Store the action state info for later
    InputState::ActionStateInfo leftActionStateInfo {
            grabAction,
            handSubactionPath[Side::LEFT],
            XrActionStateFloat{}
    };

    InputState::ActionStateInfo rightActionStateInfo {
            grabAction,
            handSubactionPath[Side::RIGHT],
            XrActionStateFloat{}
    };

    actionStateInfoMap.at(ActionName::LeftGrab) = leftActionStateInfo;
    actionStateInfoMap.at(ActionName::RightGrab) = rightActionStateInfo;
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

void InputState::UpdateActionStateFloat(ActionName actionName) {
//    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
//    getInfo.action = actionStateInfoMap.at(actionName).first;
//    getInfo.subactionPath = actionStateInfoMapMap.at(actionName).second;
//
//    XrActionStateFloat grabValue{XR_TYPE_ACTION_STATE_FLOAT};
//    CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &grabValue));
}

void InputState::CreatePoseActions() {
    // Create an input action getting the left and right hand poses.
    XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
    actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
    strcpy_s(actionInfo.actionName, "hand_pose");
    strcpy_s(actionInfo.localizedActionName, "Hand Pose");
    actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
    actionInfo.subactionPaths = handSubactionPath.data();
    CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &poseAction));
}

void InputState::CreateVibrateActions() {
//    // Create output actions for vibrating the left and right controller.
//    XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
//    actionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
//    strcpy_s(actionInfo.actionName, "vibrate_hand");
//    strcpy_s(actionInfo.localizedActionName, "Vibrate Hand");
//    actionInfo.countSubactionPaths = uint32_t(input.handSubactionPath.size());
//    actionInfo.subactionPaths = input.handSubactionPath.data();
//    CHECK_XRCMD(xrCreateAction(input.actionSet, &actionInfo, &input.vibrateAction));
}

void InputState::CreateQuitAction() {
//    // Create input actions for quitting the session using the left and right controller.
//    // Since it doesn't matter which hand did this, we do not specify subaction paths for it.
//    // We will just suggest bindings for both hands, where possible.
//    actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
//    strcpy_s(actionInfo.actionName, "quit_session");
//    strcpy_s(actionInfo.localizedActionName, "Quit Session");
//    actionInfo.countSubactionPaths = 0;
//    actionInfo.subactionPaths = nullptr;
//    CHECK_XRCMD(xrCreateAction(input.actionSet, &actionInfo, &input.quitAction));
}
}