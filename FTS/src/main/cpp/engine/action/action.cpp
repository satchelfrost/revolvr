#include <action/action.h>
#include <common.h>

#include <utility>

namespace rvr {
Action::Action(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath, std::string actionPath,
               ActionType pType, Hand handConfig) :
handSubactionPath_(handSubactionPath),
actionSet_(actionSet),
actionPath_(std::move(actionPath)),
type(pType) {
    // Determine if the action applies to left hand, right hand, both, or neither
    bothHands_ = false;
    switch (handConfig) {
        case Hand::Both:
            hands = {Hand::Left, Hand::Right};
            bothHands_ = true;
            break;
        case Hand::Left:
            hands = {Hand::Left};
            break;
        case Hand::Right:
            hands = {Hand::Right};
            break;
        case Hand::Unspecified:
            hands = {Hand::Unspecified};
            break;
        default:
            THROW(Fmt("Hand configuration %d is invalid", handConfig))
    }
}

void Action::CreateAction(XrActionType actionType, const char *actionName, const char *localizedName) {
    XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
    actionInfo.next = nullptr;
    actionInfo.actionType = actionType;
    strcpy_s(actionInfo.actionName, actionName);
    strcpy_s(actionInfo.localizedActionName, localizedName);

    if (bothHands_) {
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath_.size());
        actionInfo.subactionPaths = handSubactionPath_.data();
    }
    else {
        actionInfo.countSubactionPaths = 0;
        actionInfo.subactionPaths = nullptr;
    }

    CHECK_XRCMD(xrCreateAction(actionSet_, &actionInfo, &action_));
}

void Action::UpdateActionStateFloat(Hand hand, XrActionStateFloat &floatState, XrSession& session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = action_;
    getInfo.subactionPath = GetSubactionPath(hand);
    CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &floatState));
}

void Action::CreateActionSpace(Hand hand, XrSpace& space, XrSession& session) {
    XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    actionSpaceInfo.action = action_;
    actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
    actionSpaceInfo.subactionPath = GetSubactionPath(hand);
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &space));
}

void Action::UpdateActionIsPoseAction(Hand hand, XrActionStatePose &poseState, XrSession& session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = action_;
    getInfo.subactionPath = GetSubactionPath(hand);
    CHECK_XRCMD(xrGetActionStatePose(session, &getInfo, &poseState));
}

void Action::UpdateActionStateBool(Hand hand, XrActionStateBoolean &boolState, XrSession& session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = action_;
    getInfo.subactionPath = GetSubactionPath(hand);
    CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &boolState));
}

XrPath Action::GetSubactionPath(Hand hand) {
    if (bothHands_) {
        try {
            return handSubactionPath_.at((int)hand);
        }
        catch (std::out_of_range& e) {
            THROW(Fmt("Could not get subaction path for hand %d", hand))
        }
    }
    else {
        return XR_NULL_PATH;
    }
}

XrAction Action::GetAction() {
    if (action_ == nullptr)
        THROW("Action was nullptr")
    return action_;

}

std::string Action::GetFullActionPathStr(Hand hand) {
    switch (hand) {
        case Hand::Left:
            return "/user/hand/left" + actionPath_;
        case Hand::Right:
            return "/user/hand/right" + actionPath_;
        default:
            THROW(Fmt("Action path string does not exist for hand %d", hand))
    }
}
}