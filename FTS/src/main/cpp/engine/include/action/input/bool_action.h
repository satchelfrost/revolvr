#pragma once

#include <action/action.h>


namespace rvr {
typedef std::array<XrActionStateBoolean, (size_t)Hand::Count> HandBoolArray;

class BoolAction : public Action {
public:
    BoolAction(XrActionSet actionSet, HandPathArray handSubactionPath,
               std::string actionPath, ActionType type, Hand handConfig);
    void Update(XrSession& session) override;
    XrActionStateBoolean CurrHandState();
    XrActionStateBoolean PrevHandState();
    bool StateTurnedOn();
    bool StateTurnedOff();
private:
    HandBoolArray handState_{{{XR_TYPE_ACTION_STATE_BOOLEAN},
                              {XR_TYPE_ACTION_STATE_BOOLEAN}}};
    HandBoolArray prevHandState_{{{XR_TYPE_ACTION_STATE_BOOLEAN},
                                  {XR_TYPE_ACTION_STATE_BOOLEAN}}};
    void UpdateActionStateBool(Hand hand, XrSession& session);
};
}
