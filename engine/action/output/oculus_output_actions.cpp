#include <action/output/oculus_output_actions.h>

namespace rvr {
rvr::Vibrate::Vibrate(XrActionSet actionSet, HandPathArray handSubactionPath) :
HapticAction(actionSet, handSubactionPath, "/output/haptic", ActionType::Vibrate){}
}
