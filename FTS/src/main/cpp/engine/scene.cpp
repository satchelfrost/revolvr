#include "scene.h"
#include "ecs/component/all_components.h"
#include "ecs/ecs.h"

namespace rvr {
void Scene::Init() {
    // Setup the root node
    auto root = EntityFactory::CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial});
    root->SetName("VR_Origin");
    auto rootSpatial = ECS::GetInstance()->GetComponent<Spatial>(root);
    rootSpatial->pose.position.x = 3;

    // Left hand
    auto leftHand = EntityFactory::CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial, ComponentType::Mesh});
    auto leftTracked = ECS::GetInstance()->GetComponent<TrackedSpace>(leftHand);
    auto leftSpatial = ECS::GetInstance()->GetComponent<Spatial>(leftHand);
    leftHand->SetName("Left_Hand");
    leftTracked->trackedSpaceType = TrackedSpaceType::LeftHand;
    leftSpatial->UniformScale(0.1);
    root->AddChild(leftHand);

    // Right hand
    auto rightHand = EntityFactory::CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial, ComponentType::Mesh});
    auto rightTracked = ECS::GetInstance()->GetComponent<TrackedSpace>(rightHand);
    auto rightSpatial = ECS::GetInstance()->GetComponent<Spatial>(rightHand);
    rightHand->SetName("Right_Hand");
    rightTracked->trackedSpaceType = TrackedSpaceType::RightHand;
    rightSpatial->UniformScale(0.1);
    root->AddChild(rightHand);

    // Add pointer to right hand
    auto pointer = EntityFactory::CreateEntity({ComponentType::Spatial, ComponentType::Mesh});
    auto pointerSpatial = ECS::GetInstance()->GetComponent<Spatial>(pointer);
    pointerSpatial->scale = {0.01, 0.01, 0.5};
    pointerSpatial->pose.position = {0, 0, -0.25};
    rightHand->AddChild(pointer);

    // World origin mesh
    auto origin = EntityFactory::CreateEntity({ComponentType::Spatial, ComponentType::Mesh});
    auto originSpatial = ECS::GetInstance()->GetComponent<Spatial>(origin);
    originSpatial->UniformScale(0.5);
    originSpatial->pose.position = {0, 0.5, 0};
    root->AddChild(origin);
}

void Scene::Load(const std::string &fileName) {
}
}
