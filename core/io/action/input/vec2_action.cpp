/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <io/action/input/vec2_action.h>
#include <common.h>

#include <utility>

namespace rvr {
Vec2Action::Vec2Action(XrActionSet actionSet, HandPathArray handSubactionPath,
                       std::string actionPath, ActionType type) :
Action(actionSet, handSubactionPath, std::move(actionPath), type, Hand::Both) {
    CreateAction(XR_ACTION_TYPE_VECTOR2F_INPUT);
}

void Vec2Action::Update(XrSession &session) {
    for (auto hand : hands)
        UpdateActionStateVec2(hand, session);
}

XrActionStateVector2f Vec2Action::GetHandState(Hand hand) {
    try {
        return handState_.at((int)hand);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get handState in Vec2, No such hand %d for actionType",
                       hand, toString(type).c_str()));
    }
}

void Vec2Action::UpdateActionStateVec2(Hand hand, XrSession &session) {
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
    getInfo.subactionPath = GetSubactionPath(hand);
    getInfo.action = action_;
    CHECK_XRCMD(xrGetActionStateVector2f(session, &getInfo, &handState_[(int)hand]));
}
}