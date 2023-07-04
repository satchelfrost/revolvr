/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include <io/action/action.h>


namespace rvr {
typedef std::array<XrActionStateBoolean, (size_t)Hand::Count> HandBoolArray;

class BoolAction : public Action {
public:
    BoolAction(XrActionSet actionSet, HandPathArray handSubactionPath,
               std::string actionPath, ActionType type, Hand handConfig);
    void Update(XrSession& session) override;
    XrActionStateBoolean CurrHandState(Hand hand);
    XrActionStateBoolean PrevHandState(Hand hand);
    bool StateTurnedOn(Hand hand);
    bool StateTurnedOff(Hand hand);
private:
    HandBoolArray handState_{{{XR_TYPE_ACTION_STATE_BOOLEAN},
                              {XR_TYPE_ACTION_STATE_BOOLEAN}}};
    HandBoolArray prevHandState_{{{XR_TYPE_ACTION_STATE_BOOLEAN},
                                  {XR_TYPE_ACTION_STATE_BOOLEAN}}};
    void UpdateActionStateBool(Hand hand, XrSession& session);
};
}
