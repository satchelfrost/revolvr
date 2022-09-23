#include "scene.h"
#include "ecs/component/all_components.h"

namespace rvr {
Scene::Scene() : entityFactory_(&entityPool_, &componentPoolManager_) {
    // Setup the root node
    auto rootEntity = entityFactory_.CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial});
    rootEntity->SetName("VR_Origin");
    root_ = componentPoolManager_.GetComponent<Spatial>(rootEntity);
    root_->pose.position.x = 3;

    // Left hand
    auto leftHand = entityFactory_.CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial, ComponentType::Mesh});
    auto leftTracked = componentPoolManager_.GetComponent<TrackedSpace>(leftHand);
    auto leftSpatial = componentPoolManager_.GetComponent<Spatial>(leftHand);
    leftHand->SetName("Left_Hand");
    leftTracked->trackedSpaceType = TrackedSpaceType::LeftHand;
    leftSpatial->UniformScale(0.1);
    root_->AddChild(leftSpatial);

    // Right hand
    auto rightHand = entityFactory_.CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial, ComponentType::Mesh});
    auto rightTracked = componentPoolManager_.GetComponent<TrackedSpace>(rightHand);
    auto rightSpatial = componentPoolManager_.GetComponent<Spatial>(rightHand);
    rightHand->SetName("Right_Hand");
    rightTracked->trackedSpaceType = TrackedSpaceType::RightHand;
    rightSpatial->UniformScale(0.1);
    root_->AddChild(rightSpatial);

    // Add pointer to right hand
    auto pointer = entityFactory_.CreateEntity({ComponentType::Spatial, ComponentType::Mesh});
    auto pointerSpatial = componentPoolManager_.GetComponent<Spatial>(pointer);
    pointerSpatial->scale = {0.01, 0.01, 0.5};
    pointerSpatial->pose.position = {0, 0, -0.25};
    rightSpatial->AddChild(pointerSpatial);

    // World origin mesh
    auto origin = entityFactory_.CreateEntity({ComponentType::Spatial, ComponentType::Mesh});
    auto originSpatial = componentPoolManager_.GetComponent<Spatial>(origin);
    originSpatial->scale = {0.01, 0.01, 0.5};
    originSpatial->pose.position = {0, 0, -0.25};
    originSpatial->AddChild(root_);
}

void Scene::Load(const std::string &fileName) {
}
}
