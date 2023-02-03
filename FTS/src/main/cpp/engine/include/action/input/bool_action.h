#pragma once

#include <action/action.h>

namespace rvr {
class Quit : public Action {
public:
    Quit(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
    void Update(XrSession& session) override;
    XrActionStateBoolean boolState{XR_TYPE_ACTION_STATE_BOOLEAN};
};
}
