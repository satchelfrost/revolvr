#include "include/ecs/entity/entity.h"

namespace rvr {
Entity::Entity(int entityId, const std::vector<ComponentType>& cTypes) : id(entityId) {
    InitMask(cTypes);
}

std::vector<ComponentType> Entity::GetComponentTypes() {
    std::vector<ComponentType> componentTypes;
    for (int i = 0; i < constants::IMPLEMENTED_COMPONENTS; i++)
        if (mask_.test(i))
            componentTypes.push_back(ComponentType(i));
    return componentTypes;
}

void Entity::InitMask(const std::vector<ComponentType> &cTypes) {
    for (auto cType : cTypes)
        mask_.set((int)cType);
}

void Entity::ResetMask() {
    mask_.reset();
}
}