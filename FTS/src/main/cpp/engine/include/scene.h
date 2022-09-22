#pragma once

#include "ecs/entity/entity_factory.h"
#include "ecs/component/types/spatial.h"

namespace rvr {
class Scene {
public:
    Scene();
    void Load(const std::string& fileName);
private:
    EntityPool entityPool_;
    ComponentPoolManager componentPoolManager_;
    EntityFactory entityFactory_;
    Spatial* root_;
};
}