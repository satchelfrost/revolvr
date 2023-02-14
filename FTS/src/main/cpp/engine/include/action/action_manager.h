#pragma once

#include <pch.h>
#include <array>
#include <action/action.h>
#include <action/input/oculus_input_actions.h>
#include <action/output/oculus_output_actions.h>

namespace rvr {
class ActionManager {
public:
    ActionManager();
    ~ActionManager();

    void Init(XrInstance& instance);
    void Update(XrSession& session);
    Action* GetAction(ActionType type);
    void CreateActionSpaces(XrSession& session);
    std::vector<Action*> GetActions() { return actions_; }

    XrActionSet actionSet{XR_NULL_HANDLE};

private:
    void CreateActionSet();
    void SyncActions(XrSession& session);
    void SuggestOculusBindings();
    void CreateSubactionPaths();
    XrActionSuggestedBinding ActionSuggestedBinding(Action* action, Hand hand);

    HandPathArray handSubactionPath{{XR_NULL_PATH, XR_NULL_PATH}};
    XrInstance instance_;
    std::vector<Action*> actions_;
    static const int NUM_ACTIONS = LAST_ACTION + 1;
};
}