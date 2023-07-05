/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <pch.h>
#include <array>
#include <io/action/action.h>
#include <io/action/input/oculus_input_actions.h>
#include <io/action/output/oculus_output_actions.h>

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
    void EndSession();

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