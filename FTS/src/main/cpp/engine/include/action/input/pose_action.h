#pragma once

#include <action/action.h>

namespace rvr {
class Pose : public Action {
public:
    Pose(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath);
    ~Pose();
    void Update(XrSession& session) override;
    void CreateActionSpaces(XrSession& session);
    XrBool32 isHandActive(Hand hand);
    XrSpace GetHandSpace(Hand hand);

private:
    std::array<XrSpace, (size_t)Hand::Count> handSpace_{};
    std::array<XrActionStatePose, (size_t)Hand::Count> handState_{{{XR_TYPE_ACTION_STATE_POSE},
                                                                   {XR_TYPE_ACTION_STATE_POSE}}};
    bool actionSpacesCreated_;
};
}