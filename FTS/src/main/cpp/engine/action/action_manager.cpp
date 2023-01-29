#include <include/action/input_state.h>
#include <common.h>

namespace rvr {
InputState::InputState() {
    handScale = {{1.0f, 1.0f}};
}

void InputState::Init(XrInstance& instance) {
    instance_ = instance;

    CreateActionSet();
    CreateSubactionPaths();
    CreateGrabActions();
    CreatePoseActions();
    CreateVibrateActions();
    CreateQuitAction();
    SuggestOculusBindings();
}

void InputState::CreateActionSet() {
    XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
    strcpy_s(actionSetInfo.actionSetName, "gameplay");
    strcpy_s(actionSetInfo.localizedActionSetName, "Gameplay");
    actionSetInfo.priority = 0;
    CHECK_XRCMD(xrCreateActionSet(instance_, &actionSetInfo, &actionSet));
}

void InputState::SyncActions(XrSession& session) {
    const XrActiveActionSet activeActionSet{actionSet, XR_NULL_PATH};
    XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    CHECK_XRCMD(xrSyncActions(session, &syncInfo));
}

void InputState::CreateGrabActions() {
    XrAction grabAction = CreateAction(XR_ACTION_TYPE_FLOAT_INPUT,
                                       "grab_object",
                                       "Grab object",
                                       true);

    // Store the action state info for later
    InputState::ActionStateInfo leftActionStateInfo{};
    leftActionStateInfo.action = grabAction;
    leftActionStateInfo.subactionPath = handSubactionPath[Side::LEFT];
    leftActionStateInfo.floatState = XrActionStateFloat{XR_TYPE_ACTION_STATE_FLOAT};
    actionStateInfoMap[ActionName::LeftGrab] = leftActionStateInfo;

    InputState::ActionStateInfo rightActionStateInfo{};
    rightActionStateInfo.action = grabAction;
    rightActionStateInfo.subactionPath = handSubactionPath[Side::RIGHT];
    rightActionStateInfo.floatState = XrActionStateFloat{XR_TYPE_ACTION_STATE_FLOAT};
    actionStateInfoMap[ActionName::RightGrab] = rightActionStateInfo;
}

void InputState::VibrateWithAmplitude(float amplitude, XrSession& session, int hand) {
    XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
    hapticActionInfo.action = GetAction(ActionName::LeftVibrate);
    hapticActionInfo.subactionPath = handSubactionPath[hand];
    XrHapticVibration vibration;
    if (hand == Side::LEFT)
        vibration = actionStateInfoMap.at(ActionName::LeftVibrate).vibration;
    else
        vibration = actionStateInfoMap.at(ActionName::RightVibrate).vibration;

    CHECK_XRCMD(xrApplyHapticFeedback(session,
                                           &hapticActionInfo,
                                           (XrHapticBaseHeader*)&vibration));
}

void InputState::UpdateActionStateFloat(ActionName actionName, XrSession& session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = GetAction(actionName);
    getInfo.subactionPath = actionStateInfoMap.at(actionName).subactionPath;
    CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &actionStateInfoMap.at(actionName).floatState));
}

void InputState::CreatePoseActions() {
    XrAction poseAction = CreateAction(XR_ACTION_TYPE_POSE_INPUT,
                                       "hand_pose",
                                       "Hand Pose",
                                       true);

    // Store the action state info for later
    InputState::ActionStateInfo leftActionStateInfo{};
    leftActionStateInfo.action = poseAction;
    leftActionStateInfo.subactionPath = handSubactionPath[Side::LEFT];
    leftActionStateInfo.poseState = XrActionStatePose{XR_TYPE_ACTION_STATE_POSE};
    actionStateInfoMap[ActionName::LeftPose] = leftActionStateInfo;

    InputState::ActionStateInfo rightActionStateInfo{};
    rightActionStateInfo.action = poseAction;
    rightActionStateInfo.subactionPath = handSubactionPath[Side::RIGHT];
    rightActionStateInfo.poseState = XrActionStatePose{XR_TYPE_ACTION_STATE_POSE};
    actionStateInfoMap[ActionName::RightPose] = rightActionStateInfo;
}

void InputState::CreateVibrateActions() {
    XrAction vibrateAction = CreateAction(XR_ACTION_TYPE_VIBRATION_OUTPUT,
                                          "vibrate_hand",
                                          "Vibrate Hand",
                                          true);
    XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
    vibration.amplitude = 0.0;
    vibration.duration = XR_MIN_HAPTIC_DURATION;
    vibration.frequency = XR_FREQUENCY_UNSPECIFIED;

    // Store the action state info for later
    InputState::ActionStateInfo leftActionStateInfo{};
    leftActionStateInfo.action = vibrateAction;
    leftActionStateInfo.subactionPath = handSubactionPath[Side::LEFT];
    leftActionStateInfo.vibration = vibration;
    actionStateInfoMap[ActionName::LeftVibrate] = leftActionStateInfo;

    InputState::ActionStateInfo rightActionStateInfo{};
    rightActionStateInfo.action = vibrateAction;
    rightActionStateInfo.subactionPath = handSubactionPath[Side::RIGHT];
    rightActionStateInfo.vibration = vibration;
    actionStateInfoMap[ActionName::RightVibrate] = rightActionStateInfo;
}

void InputState::CreateQuitAction() {
    XrAction quitAction = CreateAction(XR_ACTION_TYPE_BOOLEAN_INPUT,
                                       "quit_session",
                                       "Quit Session",
                                       false);
    InputState::ActionStateInfo actionStateInfo{};
    actionStateInfo.action = quitAction;
    actionStateInfo.boolState = XrActionStateBoolean{XR_TYPE_ACTION_STATE_BOOLEAN};
    actionStateInfoMap[ActionName::Quit] = actionStateInfo;
}

XrAction InputState::CreateAction(XrActionType actionType, const char *actionName, const char *localizedName, bool bothHands) {
    XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
    actionInfo.next = nullptr;
    actionInfo.actionType = actionType;
    strcpy_s(actionInfo.actionName, actionName);
    strcpy_s(actionInfo.localizedActionName, localizedName);

    // Some actions we don't specify both hands e.g. quit action
    if (bothHands) {
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
    }
    else {
        actionInfo.countSubactionPaths = 0;
        actionInfo.subactionPaths = nullptr;
    }

    XrAction action;
    CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &action));
    return action;
}

void InputState::UpdateInputActions(XrSession& session) {
    UpdateActionStateFloat(ActionName::LeftGrab, session);
    UpdateActionStateFloat(ActionName::RightGrab, session);
    UpdateActionStateBool(ActionName::Quit, session);
}

void InputState::CreateActionSpace(XrSession& session) {
    XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    actionSpaceInfo.action = GetAction(ActionName::LeftPose);
    actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
    actionSpaceInfo.subactionPath = actionStateInfoMap.at(ActionName::LeftPose).subactionPath;
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &handSpace[Side::LEFT]));


    actionSpaceInfo.action = GetAction(ActionName::RightPose);
    actionSpaceInfo.subactionPath = actionStateInfoMap.at(ActionName::RightPose).subactionPath;
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &handSpace[Side::RIGHT]));
}

void InputState::SuggestOculusBindings() {
    XrPath oculusTouchInteractionProfilePath;
    CHECK_XRCMD(xrStringToPath(instance_,
                                    "/interaction_profiles/oculus/touch_controller",
                                    &oculusTouchInteractionProfilePath));
    std::vector<XrActionSuggestedBinding> bindings;
    bindings.emplace_back(ActionSuggestedBinding(ActionName::LeftGrab, "/user/hand/left/input/squeeze/value"));
    bindings.emplace_back(ActionSuggestedBinding(ActionName::RightGrab, "/user/hand/right/input/squeeze/value"));
    bindings.emplace_back(ActionSuggestedBinding(ActionName::LeftPose, "/user/hand/left/input/grip/pose"));
    bindings.emplace_back(ActionSuggestedBinding(ActionName::RightPose, "/user/hand/right/input/grip/pose"));
    bindings.emplace_back(ActionSuggestedBinding(ActionName::Quit, "/user/hand/left/input/menu/click"));
    bindings.emplace_back(ActionSuggestedBinding(ActionName::LeftVibrate, "/user/hand/left/output/haptic"));
    bindings.emplace_back(ActionSuggestedBinding(ActionName::RightVibrate, "/user/hand/right/output/haptic"));
    XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
    suggestedBindings.interactionProfile = oculusTouchInteractionProfilePath;
    suggestedBindings.suggestedBindings = (const XrActionSuggestedBinding*)bindings.data();
    suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
    CHECK_XRCMD(xrSuggestInteractionProfileBindings(instance_, &suggestedBindings));
}

void InputState::CreateSubactionPaths() {
    CHECK_XRCMD(xrStringToPath(instance_, "/user/hand/left", &handSubactionPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance_, "/user/hand/right", &handSubactionPath[Side::RIGHT]));
}

XrActionSuggestedBinding InputState::ActionSuggestedBinding(ActionName actionName, const char* bindingString) {
    XrPath bindingPath = XR_NULL_PATH;
    CHECK_XRCMD(xrStringToPath(instance_, bindingString, &bindingPath));
    return XrActionSuggestedBinding{GetAction(actionName), bindingPath};
}

void InputState::UpdateActionStateBool(ActionName actionName, XrSession &session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = GetAction(actionName);
    getInfo.subactionPath = actionStateInfoMap.at(actionName).subactionPath;
    CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &actionStateInfoMap.at(actionName).boolState));
}

XrAction InputState::GetAction(ActionName actionName) {
    XrAction action;
    try {
        action = actionStateInfoMap.at(actionName).action;
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Action not found for action name %d", (int)actionName));
    }
    return action;
}

void InputState::Update(XrSession &session) {
    SyncActions(session);
    UpdateInputActions(session);
}
}