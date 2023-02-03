#pragma once

#include <action/action.h>

namespace rvr {
class FloatAction : public Action {
public:
    FloatAction(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath,
                std::string actionPath, ActionType type);
    void Update(XrSession& session) override;
    XrActionStateFloat GetHandState(Hand hand);
private:
    void UpdateActionStateFloat(Hand hand, XrSession& session);
    std::array<XrActionStateFloat, (size_t)Hand::Count> handState_{{{XR_TYPE_ACTION_STATE_FLOAT},
                                                                    {XR_TYPE_ACTION_STATE_FLOAT}}};
};
}