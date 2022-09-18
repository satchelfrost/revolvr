#pragma once

#include "ecs/ecs.h"
#include "ecs/entity/entity_pool.h"
#include "ecs/entity/entity_factory.h"
#include "ecs/entity/entity_node.h"
#include "ecs/component/component_pool_manager.h"


namespace rvr {
class Scene {
public:
    Scene();
    void Load(const std::string& fileName);
private:
    EntityPool entityPool_;
    ComponentPoolManager componentPoolManager_;
    EntityFactory entityFactory_;
};
}