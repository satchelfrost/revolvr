#pragma once

#include "entity.h"

namespace rvr {
class EntityNode {
public:
    EntityNode(Entity *entity);
    void AddChild(Entity *entity);
    void SetParent(Entity *entity);
    EntityNode *GetParent() { return parent_; }
    std::list<EntityNode*>& GetChildren() { return children_; }
    void Destroy(); // TODO: Implementation of this should be different since we only want to deactivate entities not delete them.
    void SetName(std::string name);

    std::string GetName();

private:
    void RemoveFromParent();
    void GenerateAndSetName();

    EntityNode *parent_;
    std::list<EntityNode*> children_;
    std::string name_;
    Entity *entity_;
};
}