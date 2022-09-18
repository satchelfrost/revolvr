#include "ecs/component/component_pool.h"

namespace rvr {
ComponentPool::ComponentPool(ComponentType cType) : poolType_(cType) {
    components_.resize(constants::MAX_ENTITIES);
}

ComponentPool::~ComponentPool() {
    for (auto& component : components_)
        delete component;
}

bool ComponentPool::CreateComponent(type::EntityId entityId) {
    Component* component = nullptr;
    switch (poolType_) {
        case ComponentType::Spatial:
            component = new Spatial();
            break;
        case ComponentType::Mesh:
        case ComponentType::Origin:
        case ComponentType::Hand:
            break;
    }

    if (!component) {
        Log::Write(Log::Level::Warning, Fmt("Component id %d has no implementation", poolType_));
        return false;
    }

    components_.at(entityId) = component;
    return true;
}
}

