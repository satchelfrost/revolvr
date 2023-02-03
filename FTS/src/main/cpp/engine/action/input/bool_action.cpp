#include <action/input/bool_action.h>
#include <common.h>

namespace rvr {
BoolAction::BoolAction(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath,
                       std::string actionPath, ActionType type, Hand handConfig) :
Action(actionSet, handSubactionPath, std::move(actionPath), type, handConfig) {
    CreateAction(XR_ACTION_TYPE_BOOLEAN_INPUT);
}

void BoolAction::Update(XrSession& session) {
    for (auto hand : hands)
        UpdateActionStateBool(hand, session);
}

void BoolAction::UpdateActionStateBool(Hand hand, XrSession& session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = action_;
    getInfo.subactionPath = GetSubactionPath(hand);
    CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &handState_[(int)hand]));
}

XrActionStateBoolean BoolAction::GetHandState(Hand hand) {
    try {
        return handState_.at((int)hand);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get handState in Boolean, No such hand %d", hand));
    }
}
}
