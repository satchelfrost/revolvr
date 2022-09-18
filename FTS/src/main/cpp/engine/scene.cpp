#include "scene.h"

namespace rvr {
Scene::Scene() : entityFactory_(&entityPool_, &componentPoolManager_) {}

void Scene::Load(const std::string &fileName) {

}
}
