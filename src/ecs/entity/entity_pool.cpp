/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include "ecs/entity/entity_pool.h"
#include "ecs/ecs_info.h"
#include "common.h"

namespace rvr {
EntityPool::EntityPool() : highestEntityId_(-1) {
    entities_.resize(constants::MAX_ENTITIES);
}

EntityPool::~EntityPool() {
    for (auto entity : entities_)
        delete entity;
}

Entity *EntityPool::CreateNewEntity(bool setRootAsParent) {
    // First recycle old entities if possible
    if (!inactiveIds_.empty()) {
        auto index = inactiveIds_.back();
        inactiveIds_.pop_back();
        auto entity = entities_.at(index);
        CHECK_MSG(entity, Fmt("Expected inactive entity at index %d, found nullptr instead.", index))

        if (setRootAsParent)
            GetRoot()->AddChild(entity);

        return entity;
    }
    return CreateNewEntity(++highestEntityId_, setRootAsParent);
}

Entity *EntityPool::CreateNewEntity(type::EntityId id, bool setRootAsParent) {
    // Max entity check
    if (id >= constants::MAX_ENTITIES)
        THROW(Fmt("[Entity id %d, MAX_ENTITIES %d] - Adjust max entities in ecs_info.h.",
                        id,
                        constants::MAX_ENTITIES));

    // Ensure entity does not already exist
    if(entities_.at(id) != nullptr)
        THROW(Fmt("[Entity id %d] - Already exists.", id));

    // Highest id so far
    if (id > highestEntityId_)
        highestEntityId_ = id;

    // If no available entities create one
    auto entity = new Entity(id);
    entities_.at(id) = entity;
    if (setRootAsParent)
        GetRoot()->AddChild(entity);

    return entity;
}

void EntityPool::FreeEntity(int entityId) {
    CHECK_MSG((entityId != 0), "Unable to free root node.")
    Entity* entity = entities_.at(entityId);
    CHECK_MSG(entity, "Attempting to free nonexistent entity.")
    entity->ResetMask();
    inactiveIds_.push_back(entity->id);
}

Entity *EntityPool::GetRoot() {
    Entity* entity = GetEntity(constants::ROOT_ID);
    CHECK_MSG((entity->id == constants::ROOT_ID), "rvr::EntityPool::entities_[ROOT_ID]->id != ROOT_ID")
    return entity;
}

Entity *EntityPool::GetEntity(type::EntityId id) {
    Entity* entity = entities_.at(id);
    CHECK_MSG(entity, Fmt("Entity %d is nullptr", id))
    return entity;
}

void EntityPool::FillHoles() {
    for (type::EntityId id = 0; id < highestEntityId_; id++) {
        if (entities_.at(id) == nullptr) {
            // create entity without any components
            entities_.at(id) = new Entity(id);
            // push it into the inactive entities list
            inactiveIds_.push_back(id);
        }
    }
}
}