#pragma once

#include <action/action.h>

namespace rvr {
class Grab : public Action {
public:
    Grab(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
    void Update(XrSession& session) override;
    XrActionStateFloat GetHandState(Hand hand);
private:
    std::array<XrActionStateFloat, (size_t)Hand::Count> handState_{{{XR_TYPE_ACTION_STATE_FLOAT},
                                                                    {XR_TYPE_ACTION_STATE_FLOAT}}};
};
}