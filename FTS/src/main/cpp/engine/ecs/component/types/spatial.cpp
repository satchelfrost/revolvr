#include "include/ecs/component/types/spatial.h"

namespace rvr {
Spatial::Spatial() : Component(ComponentType::Spatial),
                     pose(RVRMath::Pose::Identity()),
                     worldPose(RVRMath::Pose::Identity()),
                     scale(RVRMath::Vector::One()),
                     parent_(nullptr) {}

void Spatial::AddChild(Spatial* child) {
    child->SetParent(this);
    children_.push_back(child);
}

void Spatial::RemoveFromParent() {
    if (parent_) {
        auto& children = parent_->GetChildren();
        auto childItr = std::find(children.begin(), children.end(), this);
        CHECK(childItr != children.end());
        children.erase(childItr);
    }
}

void Spatial::SetParent(Spatial* parent) {
    RemoveFromParent();
    parent_ = parent;
}

const Spatial* Spatial::GetParent() {
    return parent_;
}

std::list<Spatial*>& Spatial::GetChildren() {
    return children_;
}

void Spatial::UniformScale(float scaleFactor) {
    scale.x = scale.x * scaleFactor;
    scale.y = scale.y * scaleFactor;
    scale.z = scale.z * scaleFactor;
}
}