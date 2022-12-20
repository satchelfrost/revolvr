#include "include/ecs/entity/entity.h"
#include "include/ecs/ecs.h"
#include "check.h"

namespace rvr {
Entity::Entity(int entityId) : id(entityId), parent_(nullptr) {}

std::vector<ComponentType> Entity::GetComponentTypes() {
    std::vector<ComponentType> componentTypes;
    for (int i = 0; i < constants::IMPLEMENTED_COMPONENTS; i++)
        if (mask_.test(i))
            componentTypes.push_back(ComponentType(i));
    return componentTypes;
}

void Entity::InitMask(const std::vector<ComponentType> &cTypes) {
    for (auto cType : cTypes)
        mask_.set((int)cType);
}

void Entity::ResetMask() {
    mask_.reset();
}

void Entity::GenerateAndSetName() {
    name_ = "entity_" + std::to_string(id);
}

std::string Entity::GetName() {
    if (!name_.empty()) {
        return name_;
    }
    else {
        GenerateAndSetName();
        return name_;
    }
}

void Entity::SetName(std::string name) {
    name_ = std::move(name);
}

bool Entity::HasComponent(ComponentType cType) {
    return mask_.test((int)cType);
}

void Entity::AddChild(Entity* child) {
    child->SetParent(this);
    children_.push_back(child);
}

void Entity::RemoveFromParent() {
    if (parent_) {
        auto& children = parent_->GetChildren();
        auto childItr = std::find(children.begin(), children.end(), this);
        CHECK(childItr != children.end());
        children.erase(childItr);
    }
}

void Entity::SetParent(Entity* parent) {
    RemoveFromParent();
    parent_ = parent;
}

Entity* Entity::GetParent() {
    return parent_;
}

std::list<Entity*>& Entity::GetChildren() {
    return children_;
}

void Entity::Destroy() {
    RemoveFromParent();
    for (auto child : GetChildren()) {
        child->Destroy();
    }
    FreeComponents();
    ECS::Instance()->FreeEntity(id);
}

bool Entity::Active() {
    return mask_.any();
}

void Entity::FreeComponents() {
    for (auto componentType : GetComponentTypes())
        ECS::Instance()->GetPool(componentType)->FreeComponent(id);
}

void Entity::SetMask(int bit) {
    mask_.set(bit);
}
}