#include <action/input/pose_action.h>
#include <common.h>

namespace rvr {
PoseAction::PoseAction(XrActionSet actionSet, HandPathArray handSubactionPath,
                       std::string actionPath, ActionType type) :
Action(actionSet, handSubactionPath, std::move(actionPath), type, Hand::Both),
actionSpacesCreated_(false) {
    CreateAction(XR_ACTION_TYPE_POSE_INPUT);
}

PoseAction::~PoseAction() {
    if (actionSpacesCreated_) {
        for (Hand hand : hands)
            xrDestroySpace(handSpace_[(int)hand]);
    }
}

void PoseAction::Update(XrSession& session) {
    if (!actionSpacesCreated_)
        return;
    for (Hand hand : hands)
        UpdateActionStatePose(hand, session);
}

void PoseAction::CreateActionSpaces(XrSession &session) {
    for (Hand hand : hands)
        CreateActionSpace(hand, session);
    actionSpacesCreated_ = true;
}

void PoseAction::CreateActionSpace(Hand hand, XrSession& session) {
    XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    actionSpaceInfo.action = action_;
    actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
    actionSpaceInfo.subactionPath = GetSubactionPath(hand);
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &handSpace_[(int)hand]));
}

XrBool32 PoseAction::IsHandActive(Hand hand) {
try {
    return handState_.at((int)hand).isActive;
}
catch (std::out_of_range& e) {
    THROW(Fmt("Attempted to get handState in Pose, No such hand %d for actionType %s",
                   hand, toString(type).c_str()));
}
}

XrSpace PoseAction::GetHandSpace(Hand hand) {
    try {
        return handSpace_.at((int)hand);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get handSpace in Pose, No such hand %d for actionType %s",
                       hand, toString(type).c_str()));
    }
}

void PoseAction::UpdateActionStatePose(Hand hand, XrSession& session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = action_;
    getInfo.subactionPath = GetSubactionPath(hand);
    CHECK_XRCMD(xrGetActionStatePose(session, &getInfo, &handState_[(int)hand]));
}
}