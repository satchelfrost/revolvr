#include "ecs/entity/entity_pool.h"
#include "ecs/ecs_info.h"
#include "common.h"

namespace rvr {
EntityPool::EntityPool() : nextEntityId_(0) {
    entities_.resize(constants::MAX_ENTITIES);
}

EntityPool::~EntityPool() {
    for (auto entity : entities_)
        delete entity;
}

Entity *EntityPool::GetNewEntity(const std::vector<ComponentType> &cTypes) {
    // First recycle old entities if possible
    if (!inactiveIds_.empty()) {
        auto index = inactiveIds_.back();
        inactiveIds_.pop_back();
        auto entity = entities_.at(index);
        CHECK_MSG(entity, Fmt("Expected inactive entity at index %d, found nullptr instead.", index))
        entity->InitMask(cTypes);
        return entity;
    }

    // If no available entities create one
    auto entity = new Entity(nextEntityId_++, cTypes);
    if (entity->id >= constants::MAX_ENTITIES)
        THROW(Fmt("[Entity id %d, MAX_ENTITIES %d] - Adjust max entities in ecs_info.h.", entity->id, constants::MAX_ENTITIES));
    entities_.at(entity->id) = entity;
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
}