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
    LeftPose = 2,
    RightPose = 3,
    Quit = 4,
    LeftVibrate = 5,
    RightVibrate = 6
};

class InputState {
public:
    InputState();

    void Init(XrInstance& instance);
    void Update(XrSession& session);

    // might make this accessible to friends instead of public
    void VibrateWithAmplitude(float amplitude, XrSession& session, int hand);
    XrAction GetAction(ActionName actionName);

    // Action set
    XrActionSet actionSet{XR_NULL_HANDLE};

    // Hand specifics
    std::array<XrPath, Side::COUNT> handSubactionPath{};
    std::array<XrSpace, Side::COUNT> handSpace{};
    std::array<float, Side::COUNT> handScale;
    std::array<XrBool32, Side::COUNT> handActive{};

    // Other paths
//    std::array<XrPath, Side::COUNT> selectPath;
//    std::array<XrPath, Side::COUNT> squeezeValuePath;
//    std::array<XrPath, Side::COUNT> squeezeForcePath;
//    std::array<XrPath, Side::COUNT> squeezeClickPath;
//    std::array<XrPath, Side::COUNT> posePath;
//    std::array<XrPath, Side::COUNT> hapticPath;
//    std::array<XrPath, Side::COUNT> menuClickPath;
//    std::array<XrPath, Side::COUNT> bClickPath;
//    std::array<XrPath, Side::COUNT> triggerValuePath;

    // Action info stores action state information
    struct ActionStateInfo {
        XrAction action;
        XrPath subactionPath;

        union {
            XrActionStateFloat floatState;
            XrActionStatePose poseState;
            XrActionStateBoolean boolState;
            XrActionStateVector2f vec2State;
            XrHapticVibration vibration;
        };
    };

    // Maps an action name to its ActionInformation
    std::map<ActionName, ActionStateInfo> actionStateInfoMap;

    void CreateActionSpace(XrSession& session);
private:
    // Create methods
    void CreateActionSet();
    void CreateGrabActions();
    void CreatePoseActions();
    void CreateVibrateActions();
    void CreateQuitAction();
    XrAction CreateAction(XrActionType actionType, const char* actionName, const char* localizedName, bool bothHands);

    // Internal updates
    void UpdateInputActions(XrSession& session);
    void UpdateActionStateFloat(ActionName actionName, XrSession& session);
    void UpdateActionStateBool(ActionName actionName, XrSession& session);
    void SyncActions(XrSession& session);

    void SuggestOculusBindings();
    void CreateSubactionPaths();

    XrActionSuggestedBinding ActionSuggestedBinding(ActionName actionName, const char* binding);

    XrInstance instance_;
};
}