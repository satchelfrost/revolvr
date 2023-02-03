#pragma once

#include <action/action.h>

namespace rvr {
typedef std::array<XrActionStateVector2f, (size_t)Hand::Count> HandVec2Array;

class Vec2Action : public Action {
public:
    Vec2Action(XrActionSet actionSet, HandPathArray handSubactionPath,
               std::string actionPath, ActionType type);
    void Update(XrSession& session) override;
    XrActionStateVector2f GetHandState(Hand hand);

private:
    HandVec2Array handState_{{{XR_TYPE_ACTION_STATE_VECTOR2F},
                              {XR_TYPE_ACTION_STATE_VECTOR2F}}};
    void UpdateActionStateVec2(Hand hand, XrSession& session);
};
}
