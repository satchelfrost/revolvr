/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <io/action/action_manager.h>
#include <common.h>

#define POPULATE_ACTIONS(ENUM, NUM) actions_[(int)ActionType::ENUM] = new ENUM(actionSet, handSubactionPath);

namespace rvr {
ActionManager::ActionManager() {
    actions_.resize(NUM_ACTIONS);
}

ActionManager::~ActionManager() {
//    for (auto action : actions_)
//        delete action;
}

void ActionManager::Init(XrInstance& instance) {
    instance_ = instance;

    CreateActionSet();
    CreateSubactionPaths();
    ACTION_LIST(POPULATE_ACTIONS)
    SuggestOculusBindings();
}

void ActionManager::CreateActionSet() {
    XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
    strcpy_s(actionSetInfo.actionSetName, "gameplay");
    strcpy_s(actionSetInfo.localizedActionSetName, "Gameplay");
    actionSetInfo.priority = 0;
    CHECK_XRCMD(xrCreateActionSet(instance_, &actionSetInfo, &actionSet));
}

void ActionManager::CreateSubactionPaths() {
    CHECK_XRCMD(xrStringToPath(instance_, "/user/hand/left", &handSubactionPath[(int)Hand::Left]));
    CHECK_XRCMD(xrStringToPath(instance_, "/user/hand/right", &handSubactionPath[(int)Hand::Right]));
}

void ActionManager::SuggestOculusBindings() {
    XrPath oculusTouchInteractionProfilePath;
    CHECK_XRCMD(xrStringToPath(instance_,
                                    "/interaction_profiles/oculus/touch_controller",
                                    &oculusTouchInteractionProfilePath));

    // Setup action bindings for each hand if possible
    std::vector<XrActionSuggestedBinding> bindings;
    for (auto action : actions_)
        for (auto hand : action->hands)
            bindings.emplace_back(ActionSuggestedBinding(action, hand));

    XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
    suggestedBindings.interactionProfile = oculusTouchInteractionProfilePath;
    suggestedBindings.suggestedBindings = (const XrActionSuggestedBinding*)bindings.data();
    suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
    CHECK_XRCMD(xrSuggestInteractionProfileBindings(instance_, &suggestedBindings));
}

XrActionSuggestedBinding ActionManager::ActionSuggestedBinding(Action* action, Hand hand) {
    XrPath bindingPath = XR_NULL_PATH;
    CHECK_XRCMD(xrStringToPath(instance_, action->GetFullActionPathStr(hand).c_str(), &bindingPath));
    return XrActionSuggestedBinding{action->GetAction(), bindingPath};
}

void ActionManager::Update(XrSession &session) {
    SyncActions(session);
    for (auto action : actions_)
        action->Update(session);
}

void ActionManager::SyncActions(XrSession& session) {
    const XrActiveActionSet activeActionSet{actionSet, XR_NULL_PATH};
    XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    CHECK_XRCMD(xrSyncActions(session, &syncInfo));
}

void ActionManager::CreateActionSpaces(XrSession &session) {
    auto gripPose = dynamic_cast<GripPose*>(actions_.at((int)ActionType::GripPose));
    gripPose->CreateActionSpaces(session);

    auto aimPose = dynamic_cast<AimPose*>(actions_.at((int)ActionType::AimPose));
    aimPose->CreateActionSpaces(session);
}

Action* ActionManager::GetAction(ActionType type) {
    try {
        return actions_.at((int)type);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Failed to get action with type ActionType %d", type))
    }
}

void ActionManager::EndSession() {
    for (auto action : actions_)
        delete action;

    if (actionSet != XR_NULL_HANDLE) {
        xrDestroyActionSet(actionSet);
    }
}
}