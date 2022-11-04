#include "scene.h"
#include "ecs/component/all_components.h"
#include "ecs/ecs.h"

namespace rvr {
void Scene::Init() {
    // Setup the root node
    auto root = EntityFactory::CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial});
    root->SetName("VR_Origin");
    auto [rootSpatial, rootTracked] = ECS::Instance()->GetComponentPair<Spatial, TrackedSpace>(root->id);
    rootTracked->trackedSpaceType = TrackedSpaceType::VROrigin;
    rootSpatial->pose.position.x = 3;

    // Left hand
    auto lHand = EntityFactory::CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial, ComponentType::Mesh});
    auto [lSpatial, lTracked] = ECS::Instance()->GetComponentPair<Spatial, TrackedSpace>(lHand->id);
    lHand->SetName("Left_Hand");
    lTracked->trackedSpaceType = TrackedSpaceType::LeftHand;
    lSpatial->UniformScale(0.1);
    root->AddChild(lHand);

    // Right hand
    auto rHand = EntityFactory::CreateEntity({ComponentType::TrackedSpace, ComponentType::Spatial, ComponentType::Mesh});
    auto [rSpatial, rTracked] = ECS::Instance()->GetComponentPair<Spatial, TrackedSpace>(rHand->id);
    rHand->SetName("Right_Hand");
    rTracked->trackedSpaceType = TrackedSpaceType::RightHand;
    rSpatial->UniformScale(0.1);
    root->AddChild(rHand);

    // Add pointer to right hand
    auto pointer = EntityFactory::CreateEntity({ComponentType::Spatial, ComponentType::Mesh});
    auto pointerSpatial = ECS::Instance()->GetComponent<Spatial>(pointer->id);
    pointerSpatial->scale = {0.01, 0.01, 0.5};
    pointerSpatial->pose.position = {0, 0, -0.25};
    rHand->AddChild(pointer);

    // World origin mesh
    auto origin = EntityFactory::CreateEntity({ComponentType::Spatial, ComponentType::Mesh});
    auto originSpatial = ECS::Instance()->GetComponent<Spatial>(origin->id);
    originSpatial->UniformScale(0.5);
    originSpatial->pose.position = {0, 0.5, 0};
    root->AddChild(origin);
}

void Scene::Load(const std::string &fileName) {
}
}
