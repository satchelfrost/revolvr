#pragma once

#include "ecs/entity/entity_factory.h"

namespace rvr {
class Scene {
public:
    void Init();
    void Load(const std::string& fileName);
};
}