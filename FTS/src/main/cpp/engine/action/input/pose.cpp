#include <action/input/pose.h>
#include <common.h>

namespace rvr {
Pose::Pose(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath) :
Action(actionSet, handSubactionPath, "/input/grip/pose", ActionType::Pose, Hand::Both),
actionSpacesCreated_(false) {
    CreateAction(XR_ACTION_TYPE_POSE_INPUT,
                 "hand_pose",
                 "Hand Pose");
}

Pose::~Pose() {
    if (actionSpacesCreated_) {
        for (Hand hand : hands)
            xrDestroySpace(handSpace_[(int)hand]);
    }
}

void Pose::Update(XrSession& session) {
    if (!actionSpacesCreated_)
        return;
    for (Hand hand : hands)
        UpdateActionIsPoseAction(hand, handState_[(int)hand], session);
}

void Pose::CreateActionSpaces(XrSession &session) {
    for (Hand hand : hands)
        CreateActionSpace(hand, handSpace_[(int)hand], session);
    actionSpacesCreated_ = true;
}

XrBool32 Pose::isHandActive(Hand hand) {
    try {
        return handState_.at((int)hand).isActive;
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get handState in Pose, No such hand %d", hand));
    }
}

XrSpace Pose::GetHandSpace(Hand hand) {
    try {
        return handSpace_.at((int)hand);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get handSpace in Pose, No such hand %d", hand));
    }
}
}