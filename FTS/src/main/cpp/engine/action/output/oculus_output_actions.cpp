#include <action/output/oculus_output_actions.h>

namespace rvr {
rvr::Vibrate::Vibrate(XrActionSet actionSet, std::array<XrPath, (size_t) Hand::Count> handSubactionPath) :
HapticAction(actionSet, handSubactionPath, "/output/haptic", ActionType::Vibrate){}
}
