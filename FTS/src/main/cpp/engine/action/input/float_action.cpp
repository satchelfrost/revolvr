#include <action/input/float_action.h>
#include <common.h>

#include <utility>

namespace rvr {
FloatAction::FloatAction(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath,
                         std::string actionPath, ActionType type) :
Action(actionSet, handSubactionPath, std::move(actionPath), type, Hand::Both) {
    CreateAction(XR_ACTION_TYPE_FLOAT_INPUT);
}

void FloatAction::Update(XrSession& session) {
    for (Hand hand : hands)
        UpdateActionStateFloat(hand, session);
}

void FloatAction::UpdateActionStateFloat(Hand hand, XrSession& session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.action = action_;
    getInfo.subactionPath = GetSubactionPath(hand);
    CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &handState_[(int)hand]));
}

XrActionStateFloat FloatAction::GetHandState(Hand hand) {
    try {
        return handState_.at((int)hand);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get handState in Grab, No such hand %d", hand));
    }
}
}