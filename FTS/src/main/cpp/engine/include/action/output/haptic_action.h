#pragma once

#include <action/action.h>

namespace rvr {
class HapticAction : public Action {
public:
    HapticAction(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath,
                 std::string actionPath, ActionType type);
    void Update(XrSession& session) override;
    void Reset();
    void ApplyVibration(XrSession &session, Hand hand, float amplitude = 0.0,
                        float frequency = 0.0, XrDuration duration = XR_MIN_HAPTIC_DURATION);
private:
    XrHapticVibration vibration_{XR_TYPE_HAPTIC_VIBRATION};
};
}
