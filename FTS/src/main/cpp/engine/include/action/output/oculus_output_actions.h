#pragma once

#include <action/output/haptic_action.h>

namespace rvr {
class Vibrate : public HapticAction {
public:
    Vibrate(XrActionSet actionSet, HandPathArray handSubactionPath);
};
}