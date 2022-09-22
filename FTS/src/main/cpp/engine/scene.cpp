#include "scene.h"

#define COMPONENT(TYPE, ENTITY, MANAGER) dynamic_cast<TYPE*>(MANAGER.GetComponent(ENTITY, ComponentType::TYPE));

namespace rvr {
Scene::Scene() : entityFactory_(&entityPool_, &componentPoolManager_) {
    Entity* rootEntity = entityFactory_.CreateEntity({ComponentType::Spatial});
    root_ = COMPONENT(Spatial, rootEntity, componentPoolManager_);
}

void Scene::Load(const std::string &fileName) {

}
}
