#include "include/rvr_object.h"

#include <utility>
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

void RVRObject::GenerateAndSetName() {
    std::string typeStr = toString(type);
    std::string nameStr = typeStr + "_" + std::to_string(id);
    name_ = nameStr;
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

void RVRObject::SetName(const std::string& name) {
    name_ = name + "_" + std::to_string(id);
}