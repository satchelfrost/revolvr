#include "scene.h"
#include "ecs/component/all_components.h"

namespace rvr {
Scene::Scene() : entityFactory_(&entityPool_, &componentPoolManager_) {
    Entity* rootEntity = entityFactory_.CreateEntity({ComponentType::Spatial});
    root_ = componentPoolManager_.GetComponent<Spatial>(rootEntity);

    Entity* otherEntity = entityFactory_.CreateEntity({ComponentType::Mesh});
    auto mesh = componentPoolManager_.GetComponent<Mesh>(otherEntity);
    mesh->visible = true;
}

void Scene::Load(const std::string &fileName) {
}
}
