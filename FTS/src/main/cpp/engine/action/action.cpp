#include <action/action.h>
#include <common.h>

#define ACTION_TYPE_CASE_STR(ENUM, NUM) case ActionType::ENUM: data = #ENUM; break;

namespace rvr {
std::string toString(ActionType actionType) {
    std::string data;
    switch (actionType) {
        ACTION_LIST(ACTION_TYPE_CASE_STR)
        default:
            return "ActionType unrecognized";
    }
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c){return std::tolower(c);});
    return data;
}

Action::Action(XrActionSet actionSet, HandPathArray handSubactionPath, std::string actionPath,
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
        default:
            THROW(Fmt("Hand configuration %d is invalid", handConfig))
    }
}

void Action::CreateAction(XrActionType actionType) {
    XrActionCreateInfo actionInfo = {};
    actionInfo.type = XR_TYPE_ACTION_CREATE_INFO;
    actionInfo.next = nullptr;
    actionInfo.actionType = actionType;
    strcpy_s(actionInfo.actionName, toString(type).c_str());
    strcpy_s(actionInfo.localizedActionName, toString(type).c_str());

    if (bothHands_) {
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath_.size());
        actionInfo.subactionPaths = handSubactionPath_.data();
    }
    else {
        actionInfo.countSubactionPaths = 0;
        actionInfo.subactionPaths = nullptr;
    }

//    Log::Write(Log::Level::Info, Fmt("Action name %s", actionInfo.actionName));
    CHECK_XRCMD(xrCreateAction(actionSet_, &actionInfo, &action_));
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