#include <action/input/bool_action.h>
#include <common.h>

namespace rvr {
BoolAction::BoolAction(XrActionSet actionSet, HandPathArray handSubactionPath,
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

    // save previous state
    prevHandState_ = handState_;

    CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &handState_[(int)hand]));
}

XrActionStateBoolean BoolAction::CurrHandState(Hand hand) {
    try {
        return handState_.at((int)hand);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get handState in Boolean, No such hand for actionType %s",
                  toString(type).c_str()));
    }
}

XrActionStateBoolean BoolAction::PrevHandState(Hand hand) {
    try {
        return prevHandState_.at((int)hand);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get prevHandState in Boolean, No such hand for actionType %s",
                  toString(type).c_str()));
    }
}

bool BoolAction::StateTurnedOn(Hand hand) {
    return (!PrevHandState(hand).currentState) && (CurrHandState(hand).currentState);
}

bool BoolAction::StateTurnedOff(Hand hand) {
    return (PrevHandState(hand).currentState) && (!CurrHandState(hand).currentState);
}
}
