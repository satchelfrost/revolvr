#include <ecs/system/input_system.h>
#include <action/input/bool_action.h>

namespace rvr {
bool ButtonPressed(ActionType type) {
    Action* action;
    BoolAction* button;

    switch (type) {
        case ActionType::A:
        case ActionType::B:
            action = XrContext::Instance()->actionManager.GetAction(type);
            button = dynamic_cast<BoolAction*>(action);
            return button->StateTurnedOn(Hand::Right);
        case ActionType::X:
        case ActionType::Y:
            action = XrContext::Instance()->actionManager.GetAction(type);
            button = dynamic_cast<BoolAction*>(action);
            return button->StateTurnedOn(Hand::Left);
        default:
            THROW(Fmt("Action type %s is not a button", toString(type).c_str()));
    }
}
}