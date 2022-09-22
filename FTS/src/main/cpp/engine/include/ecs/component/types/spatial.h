#pragma once

#include "ecs/ecs.h"
#include "ecs/component/component.h"
#include "math/rvr_math.h"

namespace rvr {
class Spatial : public Component {
public:
    Spatial();
    void UniformScale(float scaleFactor);
    XrPosef pose;
    XrPosef worldPose;
    XrVector3f scale;
};
}