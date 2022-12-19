#pragma once

#include "ecs/component/component.h"
#include <ritual_behaviors.h>

namespace rvr {
class RitualBehavior {
public:
    RitualBehavior(type::EntityId pId) : id(pId) {}
    virtual ~RitualBehavior() {}
    virtual void Begin() = 0;
    virtual void Update(float delta) = 0;
    const type::EntityId id;
};

class Ritual : public Component {
public:
    Ritual();
    ~Ritual();
    void Begin() const;
    void Update(float delta) const;
    void SetImplementation(RitualBehavior* impl);
    bool HasImpl();
    bool canUpdate;
    game::RitualBehavior behavior;
    type::EntityId id;

private:
    RitualBehavior* impl_{};
};
}
