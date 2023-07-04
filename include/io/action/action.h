/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include <pch.h>
#include <array>

#define LAST_ACTION 13
#define ACTION_LIST(x)           \
    x(GripTrigger, 0)            \
    x(GripPose, 1)               \
    x(Menu, 2)                   \
    x(Vibrate, 3)                \
    x(IndexTrigger, 4)           \
    x(AimPose, 5)                \
    x(A, 6)                      \
    x(B, 7)                      \
    x(X, 8)                      \
    x(Y, 9)                      \
    x(ThumbStickTouch, 10)       \
    x(ThumbStickRestTouch, 11)   \
    x(TriggerTouch, 12)          \
    x(Joystick, LAST_ACTION)     \

#define BUILD_ENUM(ENUM, NUM) ENUM = NUM,

namespace rvr {
enum class ActionType {
    ACTION_LIST(BUILD_ENUM)
};

#undef BUILD_ENUM

std::string toString(ActionType actionType);

enum class Hand {
    Left = 0,
    Right = 1,
    Count = 2,
    Both = 3
};

typedef std::array<XrPath, (size_t)Hand::Count> HandPathArray;

class Action {
public:
    virtual void Update(XrSession& session) = 0;
    XrAction GetAction();
    std::string GetFullActionPathStr(Hand hand);
    const ActionType type;
    std::vector<Hand> hands;

    Action(XrActionSet actionSet, HandPathArray handSubactionPath,
           std::string actionPath, ActionType type, Hand handConfig);
    virtual ~Action() = default;

protected:
    void CreateAction(XrActionType actionType);
    XrPath GetSubactionPath(Hand hand);

    HandPathArray handSubactionPath_{};
    XrAction action_;
    XrActionSet actionSet_;
    std::string actionPath_;
    bool bothHands_;
};
}