#include "include/rvr_scene_tree.h"

RVRSceneTree::RVRSceneTree() {
    // Setup the root node
    nodeId_= 0;
    root_ = vrOrigin = new RVROrigin(NewId());
    vrOrigin->pose.position.x = 3;

    // Setup VR Origin
    leftHand = new RVRHand(NewId(), 0);
    rightHand = new RVRHand(NewId(), 1);
    vrOrigin->AddChild(leftHand);
    vrOrigin->AddChild(rightHand);

    // Create meshes for the hands and attach to the hands
    auto leftHandMesh = new RVRMesh(NewId());
    leftHandMesh->UniformScale(0.1);
    auto rightHandMesh = new RVRMesh(NewId());
    rightHandMesh->UniformScale(0.1);
    auto pointer = new RVRMesh(NewId());
    pointer->scale = {0.01, 0.01, 0.5};
    pointer->pose.position.z = -0.25;
    leftHand->AddChild(leftHandMesh);
    rightHand->AddChild(rightHandMesh);
    rightHand->AddChild(pointer);

    // Add four more nodes to the scene
    auto worldOriginMesh = new RVRMesh(NewId());
    worldOriginMesh->UniformScale(0.5);
    auto backMesh   = new RVRMesh(NewId());
    backMesh->scale = {0.5, 3, 0.5};
    backMesh->pose.position = {3, 1.5, 4};
    auto rightMesh  = new RVRMesh(NewId());
    rightMesh->scale = {0.5, 3, 0.5};
    rightMesh->pose.position = {6, 1.247, 0};
    rightMesh->pose.orientation = XrQuaternionf_CreateFromVectorAngle({1, 0, 0}, -3.14 / 4);
    auto frontMesh  = new RVRMesh(NewId());
    frontMesh->scale = {4, 0.5, 0.5};
    frontMesh->pose.position = {3, 0.25, -5};
    vrOrigin->AddChild(worldOriginMesh);
    vrOrigin->AddChild(backMesh);
    vrOrigin->AddChild(rightMesh);
    vrOrigin->AddChild(frontMesh);

}

int RVRSceneTree::NewId() {
    return nodeId_++;
}

void RVRSceneTree::CascadePose(const TrackedSpaceLocations& trackedSpaceLocations) {
    CascadePose_(root_, trackedSpaceLocations);
}

void RVRSceneTree::CascadePose_(RVRSpatial* parent, const TrackedSpaceLocations& trackedSpaceLocations) {
    for (auto objChild : parent->GetChildren()) {
        switch (objChild->type) {
            case RVRType::Origin: {
                auto child = reinterpret_cast<RVROrigin*>(objChild);
                child->worldPose = trackedSpaceLocations.vrOrigin.pose;
                break;
            }
            case RVRType::Hand: {
                auto child = reinterpret_cast<RVRHand*>(objChild);
                if(child->GetSide())
                    child->worldPose = trackedSpaceLocations.rightHand.pose;
                else
                    child->worldPose = trackedSpaceLocations.leftHand.pose;
                break;
            }
            case RVRType::Spatial:
            case RVRType::Mesh: {
                auto child = reinterpret_cast<RVRSpatial*>(objChild);
                XrQuaternionf_Multiply(&child->worldPose.orientation, &child->pose.orientation, &parent->worldPose.orientation);
                child->worldPose.position = parent->worldPose.position;
                XrVector3f offset = XrQuaternionf_Rotate_World(child->worldPose.orientation, child->pose.position);
                XrVector3f_Add(&child->worldPose.position, &offset, &child->worldPose.position);
                XrVector3f_Sub(&child->worldPose.position, &child->worldPose.position, &parent->pose.position);
                break;
            }
            default:
                break;
        }
        CascadePose_(reinterpret_cast<RVRSpatial*>(objChild), trackedSpaceLocations);
    }
}

std::vector<RVRMesh*> RVRSceneTree::GatherRenderables() {
    std::vector<RVRMesh*> renderables;
    GatherRenderables_(root_, renderables);
    return renderables;
}

void RVRSceneTree::GatherRenderables_(RVRObject* node, std::vector<RVRMesh*>& renderables) {
    for (auto child : node->GetChildren()) {
        if (child->type == RVRType::Mesh)
            renderables.push_back(reinterpret_cast<RVRMesh*>(child));
        GatherRenderables_(child, renderables);
    }
}