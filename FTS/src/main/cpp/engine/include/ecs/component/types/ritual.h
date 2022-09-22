#pragma once

#include "ecs/component/component.h"

namespace rvr {
class Ritual : public Component {
public:
    Ritual();
    virtual ~Ritual() {}
    virtual void Begin() {};
    virtual void Update(float delta) {};
    const bool canUpdate;
};
}
