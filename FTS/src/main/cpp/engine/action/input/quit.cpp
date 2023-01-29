#include <action/input/quit.h>

namespace rvr {
Quit::Quit(XrActionSet actionSet, std::array<XrPath, (size_t)Hand::Count> handSubactionPath) :
Action(actionSet, handSubactionPath, "/input/menu/click", ActionType::Quit, Hand::Left) {
    CreateAction(XR_ACTION_TYPE_BOOLEAN_INPUT,
                 "quit_session",
                 "Quit Session");
}

void Quit::Update(XrSession& session) {
    UpdateActionStateBool(Hand::Left, boolState, session);
}
}
