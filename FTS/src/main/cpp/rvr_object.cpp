#include "include/rvr_object.h"

#include <utility>
#include "check.h"

RVRObject::RVRObject(int pId, RVRType pType) : id(pId), type(pType), parent_(nullptr), canUpdate(true) {}

void RVRObject::AddChild(RVRObject* child) {
    child->SetParent(child);
    children_.push_back(child);
}

void RVRObject::RemoveFromParent() {
    if (parent_) {
        auto& children = parent_->GetChildren();
        auto childItr = std::find(children.begin(), children.end(), this);
        CHECK(childItr != children.end());
        children.erase(childItr);
    }
}

void RVRObject::SetParent(RVRObject* parent) {
    RemoveFromParent();
    parent_ = parent;
}

void RVRObject::Destroy() {
    RemoveFromParent();
    for (auto child : GetChildren()) {
        child->Destroy();
    }
    delete this;
}

void RVRObject::GenerateAndSetName() {
    name_ = std::string(toString(type)) + "_" + std::to_string(id);
}

std::string RVRObject::GetName() {
    if (!name_.empty()) {
        return name_;
    }
    else {
        GenerateAndSetName();
        return name_;
    }
}

void RVRObject::SetName(std::string name) {
    name_ = std::move(name);
}
