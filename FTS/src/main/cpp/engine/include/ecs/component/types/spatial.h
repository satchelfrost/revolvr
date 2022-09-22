#pragma once

#include "ecs/component/component.h"

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