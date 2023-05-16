#pragma once

#include <ecs/component/component.h>
#include <ecs/ecs_info.h>
#include <ecs/entity/entity.h>
#include <ecs/component/types/collider.h>

namespace rvr {
class Collider;

class Ritual : public Component {
public:
    Ritual(const Ritual &other) = delete;
    Ritual(const Ritual &other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    Ritual(type::EntityId id);
    virtual ~Ritual() = default;
    virtual void Begin();
    virtual void Update(float delta);
    virtual void OnTriggered(Collider* collider);
    virtual void OnTimeout();
    bool canUpdate;
};
}
