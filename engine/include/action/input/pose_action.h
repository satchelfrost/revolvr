#pragma once

#include <action/action.h>

namespace rvr {
typedef std::array<XrActionStatePose, (size_t)Hand::Count> HandPoseArray;
typedef std::array<XrSpace, (size_t)Hand::Count> HandSpaceArray;

class PoseAction : public Action {
public:
    PoseAction(XrActionSet actionSet, HandPathArray handSubactionPath,
               std::string actionPath, ActionType type);
    ~PoseAction();
    void Update(XrSession& session) override;
    void CreateActionSpaces(XrSession& session);
    XrBool32 IsHandActive(Hand hand);
    XrSpace GetHandSpace(Hand hand);

private:
    void UpdateActionStatePose(Hand hand, XrSession& session);
    void CreateActionSpace(Hand hand, XrSession& session);
    HandSpaceArray handSpace_{};
    HandPoseArray handState_{{{XR_TYPE_ACTION_STATE_POSE},
                              {XR_TYPE_ACTION_STATE_POSE}}};
    bool actionSpacesCreated_;
};
}