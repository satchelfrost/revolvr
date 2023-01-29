#pragma once

#include <pch.h>
#include <array>

namespace rvr {
enum class ActionType {
    Grab    = 0,
    Pose    = 1,
    Quit    = 2,
    Vibrate = 3
};

enum class Hand {
    Left = 0,
    Right = 1,
    Count = 2,
    Unspecified = 3,
    Both = 4
};

class Action {
public:
    virtual void Update(XrSession& session) = 0;
    XrAction GetAction();
    std::string GetFullActionPathStr(Hand hand);
    const ActionType type;
    std::vector<Hand> hands;

protected:
    Action(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath,
           std::string actionPath, ActionType type, Hand handConfig);
    void CreateAction(XrActionType actionType, const char* actionName, const char* localizedName);
    void CreateActionSpace(Hand hand, XrSpace& space, XrSession& session);
    void UpdateActionStateFloat(Hand hand, XrActionStateFloat& floatState, XrSession& session);
    void UpdateActionStateBool(Hand hand, XrActionStateBoolean& boolState, XrSession& session);
    void UpdateActionIsPoseAction(Hand hand, XrActionStatePose& poseState, XrSession& session);
    XrPath GetSubactionPath(Hand hand);

    std::array<XrPath, (size_t)Hand::Count> handSubactionPath_{};
    XrAction action_;
    XrActionSet actionSet_;
    std::string actionPath_;
    bool bothHands_;
};
}