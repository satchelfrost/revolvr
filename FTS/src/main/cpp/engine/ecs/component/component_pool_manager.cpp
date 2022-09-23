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

bool ComponentPoolManager::Assign(type::EntityId entityId, ComponentType cType) {
    // Get component pool
    auto componentPool = GetPool(cType);

    // Add the component to the pool
    return componentPool->CreateComponent(entityId);
}

ComponentPool* ComponentPoolManager::GetPool(ComponentType cType) {
    // Use the component type as index into component pools
    auto& componentPool = componentPools_.at((int)cType);

    // First check if the component pool exists
    if (!componentPool)
        componentPool = new ComponentPool(cType);

    return componentPool;
}


Component* ComponentPoolManager::GetComponent(Entity* entity, ComponentType cType) {
    ComponentPool* pool = GetPool(cType);
    return pool->GetComponent(entity->id);
}

std::vector<Component*> ComponentPoolManager::GetAllComponents(Entity *entity) {
    auto componentTypes = entity->GetComponentTypes();
    std::vector<Component*> components(componentTypes.size());
    for (auto& componentType : componentTypes)
        components.push_back(GetComponent(entity, componentType));
    return components;
}
}