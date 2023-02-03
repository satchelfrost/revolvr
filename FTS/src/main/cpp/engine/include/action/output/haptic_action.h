#pragma once

#include <action/action.h>

namespace rvr {
class Vibrate : public Action {
public:
    Vibrate(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
    void Update(XrSession& session) override;
    void Reset();
    void ApplyVibration(XrSession &session, Hand hand, float amplitude = 0.0,
                        float frequency = 0.0, XrDuration duration = XR_MIN_HAPTIC_DURATION);
private:
    XrHapticVibration vibration_{XR_TYPE_HAPTIC_VIBRATION};
};
}
