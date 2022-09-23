#pragma once

#include "ecs/entity/entity.h"

namespace rvr {
class EntityPool {
public:
    EntityPool();
    ~EntityPool();
    Entity* GetNextEntity(const std::vector<ComponentType>& cTypes);
    bool FreeEntity(int entityId);
    Entity* GetRoot();
    std::vector<Entity*> GetEntities() { return entities_; }

private:
    int nextEntityId_;
    std::vector<Entity*> entities_;
    std::vector<type::EntityId> inactiveIds_;
};
}