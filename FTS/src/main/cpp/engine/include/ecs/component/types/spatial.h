#pragma once

#include "ecs/component/component.h"

namespace rvr {
class Spatial : public Component {
public:
    Spatial(type::EntityId pId);
    void UniformScale(float scaleFactor);
    XrPosef pose;
    XrPosef worldPose;
    XrVector3f scale;
};
}