#include "ecs/component/component_pool_manager.h"
#include "ecs/component/all_components.h"

#define TYPE_TO_ENUM(ENUM, NUM) typeToEnum.insert({std::type_index(typeid(ENUM)), ComponentType::ENUM});

namespace rvr {
ComponentPoolManager::ComponentPoolManager() {
    componentPools_.resize(constants::IMPLEMENTED_COMPONENTS);
    COMPONENT_LIST(TYPE_TO_ENUM)
}

ComponentPoolManager::~ComponentPoolManager() {
    for (auto& pool : componentPools_)
        delete pool;
}

void ComponentPoolManager::Assign(Entity* entity, Component* component) {
    // Get component pool
    auto componentPool = GetPool(component->type);

    // Add the component to the pool
    componentPool->AssignComponent(entity, component);
}

ComponentPool* ComponentPoolManager::GetPool(ComponentType cType) {
    // Use the component type as index into component pools
    auto& componentPool = componentPools_.at((int)cType);

    // First check if the component pool exists
    if (!componentPool)
        componentPool = new ComponentPool(cType);

    return componentPool;
}
}