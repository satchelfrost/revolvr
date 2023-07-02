/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include "ecs/entity/entity.h"

namespace rvr {
class EntityPool {
public:
    EntityPool();
    ~EntityPool();
    Entity* CreateNewEntity(bool setRootAsParent = true);
    Entity* CreateNewEntity(type::EntityId id, bool setRootAsParent = true);
    void FreeEntity(int entityId);
    Entity* GetRoot();
    Entity* GetEntity(type::EntityId id);
    void FillHoles();

private:
    type::EntityId highestEntityId_;
    std::vector<Entity*> entities_;
    std::vector<type::EntityId> inactiveIds_;
};
}