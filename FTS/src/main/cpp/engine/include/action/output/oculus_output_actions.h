#pragma once

#include <action/output/haptic_action.h>

namespace rvr {
class Vibrate : public HapticAction {
public:
    Vibrate(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
};
}