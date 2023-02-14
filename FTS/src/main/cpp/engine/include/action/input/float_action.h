#pragma once

#include <action/action.h>

namespace rvr {
typedef std::array<XrActionStateFloat, (size_t)Hand::Count> HandFloatArray;

class FloatAction : public Action {
public:
    FloatAction(XrActionSet actionSet, HandPathArray handSubactionPath, std::string actionPath,
                ActionType type);
    void Update(XrSession& session) override;
    XrActionStateFloat GetHandState(Hand hand);
private:
    void UpdateActionStateFloat(Hand hand, XrSession& session);
    HandFloatArray handState_{{{XR_TYPE_ACTION_STATE_FLOAT},
                               {XR_TYPE_ACTION_STATE_FLOAT}}};
};
}