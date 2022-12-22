#pragma once

#include <ecs/component/component.h>
#include <ritual_type.h>
#include <ecs/ecs_info.h>

namespace rvr {

class Ritual : public Component {
public:
    Ritual(type::EntityId id);
    virtual void Begin() = 0;
    virtual void Update(float delta) = 0;
    bool canUpdate;
};
}
