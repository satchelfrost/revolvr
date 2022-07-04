#include "include/rvr_object.h"
#include "check.h"

RVRObject::RVRObject(int pId, RVRType pType) : id(pId), type(pType), parent_(nullptr), canUpdate(true) {}

void RVRObject::AddChild(RVRObject* child) {
   child->SetParent(child);
   children_.push_back(child);
}

void RVRObject::SetParent(RVRObject* parent) {
    if (parent_) {
        auto& children = parent_->GetChildren();
        auto childItr = std::find(children.begin(), children.end(), this);
        CHECK(childItr != children.end());
        children.erase(childItr);
    }

    parent_ = parent;
}