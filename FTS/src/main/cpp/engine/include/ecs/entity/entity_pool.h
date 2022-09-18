#pragma once

#include "ecs/ecs.h"
#include "ecs/entity/entity.h"
#include "logger.h"

namespace rvr {
class EntityPool {
public:
    EntityPool();
    ~EntityPool();
    Entity* GetNextEntity(const std::vector<ComponentType>& cTypes);
    bool FreeEntity(int entityId);
    Entity* GetRoot();
private:
    int nextEntityId_;
    std::vector<Entity*> entities_;
    std::vector<type::EntityId> inactiveIds_;
};
}