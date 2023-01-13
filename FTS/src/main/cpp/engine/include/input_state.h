#pragma once

#include <pch.h>
#include <array>

namespace rvr {
namespace Side {
    const int LEFT = 0;
    const int RIGHT = 1;
    const int COUNT = 2;
};

enum class ActionName {
    LeftGrab = 0,
    RightGrab = 1,
};

class InputState {
public:
    InputState();

    void SyncActions(XrSession& session);
    XrActionStateFloat GetActionStateFloat(XrAction action, XrPath subactionPath, XrSession& session);
    void VibrateWithAmplitude(float amplitude, XrSession& session, int hand);
    void UpdateActionStateFloat();


    // Action set
    XrActionSet actionSet{XR_NULL_HANDLE};

    // Various actions
    XrAction grabAction{XR_NULL_HANDLE};
    XrAction poseAction{XR_NULL_HANDLE};
    XrAction vibrateAction{XR_NULL_HANDLE};
    XrAction quitAction{XR_NULL_HANDLE};

    // Hand specifics
    std::array<XrPath, Side::COUNT> handSubactionPath{};
    std::array<XrSpace, Side::COUNT> handSpace{};
    std::array<float, Side::COUNT> handScale;
    std::array<XrBool32, Side::COUNT> handActive{};

    // Action state
    XrActionStateFloat leftGrabValue{XR_TYPE_ACTION_STATE_FLOAT};
    XrActionStateFloat rightGrabValue{XR_TYPE_ACTION_STATE_FLOAT};


    // Action info stores action state information
    struct ActionInfo {
        XrAction action;
        XrPath subactionPath;
        union {
            XrActionStateFloat trigger;
            XrActionStateBoolean pressed;
            XrActionStateVector2f analog;
        };
    };

private:
    // Maps an action name to its ActionInformation
    std::map<ActionName, ActionInfo> actionInfoMap;
};
}