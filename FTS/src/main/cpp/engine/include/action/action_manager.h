#pragma once

#include <pch.h>
#include <array>
#include <action/action.h>

#include <action/input/grab.h>
#include <action/input/pose.h>
#include <action/input/quit.h>
#include <action/output/vibrate.h>

namespace rvr {
class ActionManager {
public:
    ActionManager();
    ~ActionManager();

    void Init(XrInstance& instance);
    void Update(XrSession& session);
    Action* GetAction(ActionType type);

    // Action set
    XrActionSet actionSet{XR_NULL_HANDLE};

    // Hand specifics
    std::array<XrPath, (size_t)Hand::Count> handSubactionPath{};
    std::array<float, (size_t)Hand::Count> handScale;
    std::array<XrBool32, (size_t)Hand::Count> handActive{};

    // Todo: Does this need to be public
    void CreateActionSpace(XrSession& session);

private:
    void CreateActionSet();
    void SyncActions(XrSession& session);
    void SuggestOculusBindings();
    void CreateSubactionPaths();

    XrActionSuggestedBinding ActionSuggestedBinding(Action* action, Hand hand);

    XrInstance instance_;
    std::vector<Action*> actions_;
    static const int NUM_ACTIONS = 4;
};
}