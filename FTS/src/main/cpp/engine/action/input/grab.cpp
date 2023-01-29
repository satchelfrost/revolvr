#include <action/input/grab.h>
#include <common.h>

namespace rvr {
Grab::Grab(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath) :
Action(actionSet, handSubactionPath, "/input/squeeze/value", ActionType::Grab, Hand::Both) {
    CreateAction(XR_ACTION_TYPE_FLOAT_INPUT,
                 "grab_object",
                 "Grab object");
}

void Grab::Update(XrSession& session) {
    for (Hand hand : hands)
        UpdateActionStateFloat(hand, handState_[(int)hand], session);
}

XrActionStateFloat Grab::GetHandState(Hand hand) {
    try {
        return handState_.at((int)hand);
    }
    catch (std::out_of_range& e) {
        THROW(Fmt("Attempted to get handState in Grab, No such hand %d", hand));
    }
}
}