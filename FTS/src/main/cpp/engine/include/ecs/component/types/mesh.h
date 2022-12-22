#pragma once

#include <ecs/component/component.h>

namespace rvr {
class Mesh : public Component {
public:
    Mesh(type::EntityId pId);
    bool visible;
private:
    //Resource resource_
};
}
