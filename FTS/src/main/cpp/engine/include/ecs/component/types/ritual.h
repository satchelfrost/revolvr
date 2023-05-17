#pragma once

#include <ecs/component/component.h>
#include <ritual_type.h>
#include <ecs/ecs_info.h>
#include <ecs/entity/entity.h>
#include <ecs/component/types/collider.h>

namespace rvr {
class Collider;

class Ritual : public Component {
public:
    Ritual(type::EntityId id);
    virtual ~Ritual() = default;
    virtual Component* Clone(type::EntityId newEntityId) override;
    virtual void Begin();
    virtual void Update(float delta);
    virtual void OnTriggered(Collider* collider);
    bool canUpdate;
};
}
