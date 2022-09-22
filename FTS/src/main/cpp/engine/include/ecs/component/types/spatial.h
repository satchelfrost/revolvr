#pragma once

#include "ecs/ecs.h"
#include "ecs/component/component.h"
#include "math/rvr_math.h"
#include "check.h"

namespace rvr {
class Spatial : public Component {
public:
    Spatial();
    void AddChild(Spatial* child);
    void SetParent(Spatial* parent);
    const Spatial* GetParent();
    std::list<Spatial*>& GetChildren();
    void UniformScale(float scaleFactor);

    XrPosef pose;
    XrPosef worldPose;
    XrVector3f scale;

private:
    void RemoveFromParent();
    Spatial* parent_;
    std::list<Spatial*> children_;
};
}