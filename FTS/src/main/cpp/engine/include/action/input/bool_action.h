#pragma once

#include <action/action.h>

namespace rvr {
class BoolAction : public Action {
public:
    BoolAction(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath,
               std::string actionPath, ActionType type, Hand handConfig);
    void Update(XrSession& session) override;
    XrActionStateBoolean GetHandState(Hand hand);
private:
    std::array<XrActionStateBoolean, (size_t)Hand::Count> handState_{{{XR_TYPE_ACTION_STATE_BOOLEAN},
                                                                      {XR_TYPE_ACTION_STATE_BOOLEAN}}};
    void UpdateActionStateBool(Hand hand, XrSession& session);
};
}
