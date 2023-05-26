#include "ecs/component/component.h"

namespace rvr {
Component::Component(ComponentType pType, type::EntityId pId) : type(pType), id(pId) {}
}
