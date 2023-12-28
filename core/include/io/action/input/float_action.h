/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <io/action/action.h>

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