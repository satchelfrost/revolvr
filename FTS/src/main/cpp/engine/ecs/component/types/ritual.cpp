#include "ecs/component/types/ritual.h"

namespace rvr {
Ritual::Ritual() : Component(ComponentType::Ritual) , canUpdate(false) {}
}