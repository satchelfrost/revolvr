#include "include/rvr_scene_tree.h"

RVRSceneTree::RVRSceneTree() : nodeId_(0) {
    // Setup the root node
    root_ = static_cast<RVRObject*>(new MyRVRSpatial(NewId()));

    // Setup VR Origin
    auto vrOriginSpatial = new MyRVRSpatial(1);
    vrOriginSpatial->pose.position.x = 3;
    auto vrOrigin  = static_cast<RVRObject*>(vrOriginSpatial);
    auto leftHand  = static_cast<RVRObject*>(new MyRVRSpatial(NewId()));
    auto rightHand = static_cast<RVRObject*>(new MyRVRSpatial(NewId()));
    hands[0] = leftHand;
    hands[1] = rightHand;
    vrOrigin->AddChild(leftHand);
    vrOrigin->AddChild(rightHand);
    root_->AddChild(vrOrigin);

    // Create meshes for the hands and attach to the hands
    auto leftHandMesh = new MyRVRMesh(NewId());
    leftHandMesh->UniformScale(0.1);
    auto rightHandMesh = new MyRVRMesh(NewId());
    rightHandMesh->UniformScale(0.1);
    auto pointer = new MyRVRMesh(NewId());
    pointer->scale = {0.01, 0.01, 0.5};
    pointer->pose.position.z = -0.25;
    leftHand->AddChild(static_cast<RVRObject*>(leftHandMesh));
    rightHand->AddChild(static_cast<RVRObject*>(rightHandMesh));
    rightHand->AddChild(static_cast<RVRObject*>(pointer));

    // Add four more nodes to the scene
    auto worldOriginMesh = new MyRVRMesh(NewId());
    worldOriginMesh->UniformScale(0.5);
    auto backMesh   = new MyRVRMesh(NewId());
    backMesh->scale = {0.5, 3, 0.5};
    backMesh->pose.position = {3, 1.5, 4};
    auto rightMesh  = new MyRVRMesh(NewId());
    rightMesh->scale = {0.5, 3, 0.5};
    rightMesh->pose.position = {6, 1.247, 0};
    auto frontMesh  = new MyRVRMesh(NewId());
    frontMesh->scale = {4, 0.5, 0.5};
    frontMesh->pose.position = {3, 0.25, -5};
    root_->AddChild(static_cast<RVRObject*>(worldOriginMesh));
    root_->AddChild(static_cast<RVRObject*>(backMesh));
    root_->AddChild(static_cast<RVRObject*>(rightMesh));
    root_->AddChild(static_cast<RVRObject*>(frontMesh));

}

int RVRSceneTree::NewId() {
    return nodeId_++;
}

void RVRSceneTree::CascadePose(RVRSpatial* parent) {
    for (auto objChild : parent->GetChildren()) {
        auto child = reinterpret_cast<RVRSpatial*>(objChild);
        XrVector3f_Add(&child->pose.position, &child->pose.position, &parent->pose.position);
        child->pose.orientation = parent->pose.orientation;
        CascadePose(reinterpret_cast<RVRSpatial*>(child));
    }
}

std::vector<RVRMesh*> RVRSceneTree::GatherRenderables() {
    std::vector<RVRMesh*> renderables;
    for (auto child : root_->GetChildren()) {
        if (child->type == RVRType::RVRMesh)
            renderables.push_back(reinterpret_cast<RVRMesh*>(child));
    }
    return renderables;
}

//void RVRSceneTree::Traverse(RVRObject* node) {
//    auto children = node->GetChildren();
//    if (children.size() == 0)
//        traversedObjects_.push_back(node);
//
//    for (auto child : children)
//        Traverse(child);
//}