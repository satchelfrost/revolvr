#include "ecs/entity/entity_pool.h"

namespace rvr {
EntityPool::EntityPool() : nextEntityId_(0) {
    entities_.resize(constants::MAX_ENTITIES);
}

EntityPool::~EntityPool() {
    for (auto entity : entities_)
        delete entity;
}

Entity *EntityPool::GetNextEntity(const std::vector<ComponentType> &cTypes) {
    // First check for inactive entity IDs
    if (!inactiveIds_.empty()) {
        auto index = inactiveIds_.back();
        inactiveIds_.pop_back();
        auto entity = entities_.at(index);
        if (!entity) {
            Log::Write(Log::Level::Warning,
                       Fmt("Expected inactive entity at index %d, found nullptr instead.", index));
            entity = new Entity(index, cTypes);
        }
        entity->InitMask(cTypes);
        return entity;
    }

    // If no available entities create one
    auto entity = new Entity(nextEntityId_++, cTypes);
    entities_.at(entity->id) = entity;
    return entity;
}

bool EntityPool::FreeEntity(int entityId) {
    if (entityId == 0) {
        Log::Write(Log::Level::Warning, "Unable to free root node.");
        return false;
    }
    Entity* entity = entities_.at(entityId);
    if (!entity) {
        Log::Write(Log::Level::Warning, "Attempting to free nonexistent entity.");
        return false;
    }
    else {
        entity->ResetMask();
        inactiveIds_.push_back(entity->id);
        return true;
    }
}

Entity *EntityPool::GetRoot() {
    Entity* entity = entities_.at(constants::ROOT_ID);
    if (!entity) {
         Log::Write(Log::Level::Warning, "Root entity is nullptr");
    }
    else if (entity->id != constants::ROOT_ID) {
        Log::Write(Log::Level::Warning,
                   "rvr::EntityPool::entities_[ROOT_ID]->id != ROOT_ID");
    }
    return entity;
}
}