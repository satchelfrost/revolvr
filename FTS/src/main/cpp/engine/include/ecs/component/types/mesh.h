#pragma once

#include "ecs/ecs.h"
#include "ecs/component/component.h"

namespace rvr {
class Mesh : public Component {
public:
    Mesh();
    bool visible;
private:
    //Resource resource_
};
}
