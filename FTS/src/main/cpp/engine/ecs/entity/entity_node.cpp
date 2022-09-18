#include "include/ecs/entity/entity_node.h"

namespace rvr {
EntityNode::EntityNode(Entity* entity) : entity_(entity), parent_(nullptr) {}

void EntityNode::AddChild(Entity* entity) {
    auto child = new EntityNode(entity);
    child->SetParent(this->entity_);
    children_.push_back(child);
}

void EntityNode::RemoveFromParent() {
    if (parent_) {
        auto& children = parent_->GetChildren();
        auto childItr = std::find(children.begin(), children.end(), this);
        CHECK(childItr != children.end());
        children.erase(childItr);
    }
}

void EntityNode::SetParent(Entity* entity) {
    RemoveFromParent();
    auto parent = new EntityNode(entity);
    parent_ = parent;
}

void EntityNode::Destroy() {
    RemoveFromParent();
    for (auto child : GetChildren()) {
        child->Destroy();
    }
    delete this;
}

void EntityNode::GenerateAndSetName() {
    name_ = "entity_" + std::to_string(entity_->id);
}

std::string EntityNode::GetName() {
    if (!name_.empty()) {
        return name_;
    }
    else {
        GenerateAndSetName();
        return name_;
    }
}

void EntityNode::SetName(std::string name) {
    name_ = std::move(name);
}
}
