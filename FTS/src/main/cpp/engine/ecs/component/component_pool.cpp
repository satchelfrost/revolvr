#include "ecs/component/component_pool.h"

#define ALLOC_CASE_STR(TYPE, NUM) case ComponentType::TYPE: component = new TYPE(); break;

namespace rvr {
ComponentPool::ComponentPool(ComponentType cType) : poolType_(cType) {
    components_.resize(constants::MAX_ENTITIES);
}

ComponentPool::~ComponentPool() {
    for (auto& component : components_)
        delete component;
}

bool ComponentPool::CreateComponent(type::EntityId entityId) {
    Component* component;
    switch (poolType_) {
        COMPONENT_LIST(ALLOC_CASE_STR)
        default:
            Log::Write(Log::Level::Warning,
                       Fmt("Component %s has no implementation",
                           toString(ComponentType(poolType_))));
            return false;
    }
    components_.at(entityId) = component;
    return true;
}

Component *ComponentPool::GetComponent(type::EntityId entityId) {
    return components_.at(entityId);
}
}