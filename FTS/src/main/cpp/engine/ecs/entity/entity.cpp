#include <include/ecs/entity/entity.h>
#include <global_context.h>
#include <check.h>

namespace rvr {
Entity::Entity(int entityId) : id(entityId), parent_(nullptr) {}

std::vector<ComponentType> Entity::GetComponentTypes() {
    std::vector<ComponentType> componentTypes;
    for (int i = 0; i < constants::IMPLEMENTED_COMPONENTS; i++)
        if (mask_.test(i))
            componentTypes.push_back(ComponentType(i));
    return componentTypes;
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
        children.remove(this);
        parent_ = nullptr;
    }
    else {
        Log::Write(Log::Level::Warning, Fmt("Entity with id %d has no parent", id));
    }
}

void Entity::SetParent(Entity* parent) {
    if (parent_)
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
    if (!Active()) {
        Log::Write(Log::Level::Warning, Fmt("Attempting to destroy inactive entity %d", id));
        return;
    }

    DestroyRecursive();
    RemoveFromParent();
}

void Entity::DestroyRecursive() {
    for (auto child : GetChildren())
        child->DestroyRecursive();

    FreeComponents();
    GlobalContext::Inst()->GetECS()->FreeEntity(id);
    children_.clear();
}

bool Entity::Active() {
    return mask_.any();
}

void Entity::FreeComponents() {
    for (auto componentType : GetComponentTypes())
        GlobalContext::Inst()->GetECS()->GetPool(componentType)->FreeComponent(id);
}

void Entity::AddComponent(ComponentType cType) {
    mask_.set((int)cType);
}

Entity *Entity::Clone(type::EntityId newEntityId) {
    auto newEntity = GlobalContext::Inst()->GetECS()->CreateNewEntity(newEntityId);
    for (auto componentType : GetComponentTypes()) {
        auto component = GlobalContext::Inst()->GetECS()->GetPool(componentType)->GetComponent(id);
        auto newComponent = component->Clone(newEntityId);
        GlobalContext::Inst()->GetECS()->Assign(newEntity, newComponent);
    }
    return newEntity;
}

Entity *Entity::Clone() {
    auto newEntity = GlobalContext::Inst()->GetECS()->CreateNewEntity();
    for (auto componentType : GetComponentTypes()) {
        auto component = GlobalContext::Inst()->GetECS()->GetPool(componentType)->GetComponent(id);
        auto newComponent = component->Clone(newEntity->id);
        GlobalContext::Inst()->GetECS()->Assign(newEntity, newComponent);
    }
    return newEntity;
}
}