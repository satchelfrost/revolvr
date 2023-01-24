#pragma once

#include <ecs/component/component.h>
#include <ritual_type.h>
#include <ecs/ecs_info.h>
#include <ecs/entity/entity.h>
#include <ecs/system/notification/event.h>

namespace rvr {

class Ritual : public Component {
public:
    Ritual(type::EntityId id);
    virtual ~Ritual() = default;
    virtual void Begin() = 0;
    virtual void Update(float delta) = 0;
    virtual void OnNotify(Event* event) = 0;
    bool canUpdate;
};
}
