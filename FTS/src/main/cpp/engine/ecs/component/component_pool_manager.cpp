#include "ecs/component/component_pool_manager.h"

namespace rvr {
ComponentPoolManager::ComponentPoolManager() {
    componentPools_.resize(constants::IMPLEMENTED_COMPONENTS);
}

ComponentPoolManager::~ComponentPoolManager() {
    for (auto& pool : componentPools_)
        delete pool;
}

bool ComponentPoolManager::Assign(type::EntityId entityId, ComponentType cType) {
    // Use the component type as index into component pool
    auto componentPool = componentPools_.at((int)cType);

    // First check if the component pool exists
    if (!componentPool)
        componentPool = new ComponentPool(cType);

    // Add the component to the pool
    return componentPool->CreateComponent(entityId);
}
}