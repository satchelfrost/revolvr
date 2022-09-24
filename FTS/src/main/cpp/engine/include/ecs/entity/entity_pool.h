#pragma once

#include "ecs/entity/entity.h"

namespace rvr {
class EntityPool {
public:
    EntityPool();
    ~EntityPool();
    Entity* GetNewEntity(const std::vector<ComponentType>& cTypes);
    void FreeEntity(int entityId);
    Entity* GetRoot();
    Entity* GetEntity(type::EntityId id);

private:
    int nextEntityId_;
    std::vector<Entity*> entities_;
    std::vector<type::EntityId> inactiveIds_;
};
}