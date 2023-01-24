#pragma once

#include "ecs/entity/entity.h"

namespace rvr {
class EntityPool {
public:
    EntityPool();
    ~EntityPool();
    Entity* CreateNewEntity();
    Entity* CreateNewEntity(type::EntityId id);
    void FreeEntity(int entityId);
    Entity* GetRoot();
    Entity* GetEntity(type::EntityId id);
    void FillHoles();

private:
    type::EntityId nextEntityId_;
    std::vector<Entity*> entities_;
    std::vector<type::EntityId> inactiveIds_;
};
}