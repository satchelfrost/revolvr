#include "include/rvr_scene_tree.h"

RVRSceneTree::RVRSceneTree() {
    // Setup the root node
    nodeId_= 0;
    auto vrOrigin = root_  = new RVROrigin(NewId());
    vrOrigin->pose.position.x = 3;
    vrOrigin->SetName("VR_Origin");

    // Setup VR Origin
    auto leftHand = new RVRHand(NewId(), 0);
    leftHand->SetName("Left_Hand");
    auto rightHand = new RVRHand(NewId(), 1);
    rightHand->SetName("Right_Hand");
    vrOrigin->AddChild(leftHand);
    vrOrigin->AddChild(rightHand);

    // Create meshes for the hands and attach to the hands
    auto leftHandMesh = new RVRMesh(NewId());
    leftHandMesh->UniformScale(0.1);
    leftHandMesh->SetName("Left_Hand_Mesh");
    auto rightHandMesh = new RVRMesh(NewId());
    rightHandMesh->UniformScale(0.1);
    // Don't give rightHandMesh a name so as to see the default name generated.
    auto pointer = new SpinningPointer(NewId());
    pointer->SetName("Pointer");
    pointer->pose.position = {0, 0, -0.25};
    leftHand->AddChild(leftHandMesh);
    rightHand->AddChild(rightHandMesh);
    rightHand->AddChild(pointer);

    // Add four more nodes to the scene
    auto worldOriginMesh = new RVRMesh(NewId());
    worldOriginMesh->SetName("World_Origin_Mesh");
    worldOriginMesh->UniformScale(0.5);
    auto backMesh   = new RVRMesh(NewId());
    backMesh->SetName("Back_Mesh");
    backMesh->scale = {0.5, 3, 0.5};
    backMesh->pose.position = {3, 1.5, 4};
    auto rightMesh  = new RVRMesh(NewId());
    rightMesh->SetName("MeshOnYourRight");
    rightMesh->scale = {0.5, 3, 0.5};
    rightMesh->pose.position = {6, 1.247, 0};
    rightMesh->pose.orientation = XrQuaternionf_CreateFromVectorAngle({1, 0, 0}, -3.14 / 4);
    auto frontMesh  = new RVRMesh(NewId());
    frontMesh->SetName("The_mesh_right_in_front_of_you");
    frontMesh->scale = {4, 0.5, 0.5};
    frontMesh->pose.position = {3, 0.25, -5};
    vrOrigin->AddChild(worldOriginMesh);
    vrOrigin->AddChild(backMesh);
    vrOrigin->AddChild(rightMesh);
    vrOrigin->AddChild(frontMesh);
}

RVRSceneTree::~RVRSceneTree() {
    root_->Destroy();
}

int RVRSceneTree::NewId() {
    return nodeId_++;
}

void RVRSceneTree::CascadePose(const TrackedSpaceLocations& trackedSpaceLocations) {
    CascadePose_(root_, trackedSpaceLocations);
}

void RVRSceneTree::CascadePose_(RVRObject* parent, const TrackedSpaceLocations& trackedSpaceLocations) {
    for (auto objChild : parent->GetChildren()) {
        auto childSpatial = dynamic_cast<RVRSpatial*>(objChild);
        if(!childSpatial) // if object cannot be cast to a spatial don't cascade pose
            continue;

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
            auto parentSpatial = reinterpret_cast<RVRSpatial*>(parent);
            XrQuaternionf_Multiply(&childSpatial->worldPose.orientation, &childSpatial->pose.orientation,
                                   &parentSpatial->worldPose.orientation);
            XrVector3f offset = XrQuaternionf_Rotate(parentSpatial->worldPose.orientation, childSpatial->pose.position);
            XrVector3f_Add(&childSpatial->worldPose.position, &offset, &parentSpatial->worldPose.position);

            // Place objects relative to the origin
            if (parent->type == RVRType::Origin)
                XrVector3f_Sub(&childSpatial->worldPose.position, &childSpatial->worldPose.position,
                               &parentSpatial->pose.position);
            break;
        }
        default:
            break;
        }
        CascadePose_(objChild, trackedSpaceLocations);
    }
}

std::vector<RVRMesh*> RVRSceneTree::GatherRenderables() {
    std::vector<RVRMesh*> renderables;
    GatherRenderables_(root_, renderables);
    return renderables;
}

void RVRSceneTree::GatherRenderables_(RVRObject* parent, std::vector<RVRMesh*>& renderables) {
    for (auto objChild : parent->GetChildren()) {
        auto childSpatial = dynamic_cast<RVRSpatial*>(objChild);
        if (childSpatial) {
            if(!childSpatial->visible) {
                continue; // Children of invisible nodes will not be rendered
            }
            if (objChild->type == RVRType::Mesh) {
                auto childMesh = reinterpret_cast<RVRMesh*>(objChild);
                renderables.push_back(childMesh);
            }
            GatherRenderables_(objChild, renderables);
        }
    }
}

void RVRSceneTree::Update(float delta) {
   Update_(root_, delta);
}

void RVRSceneTree::Update_(RVRObject* parent, float delta) {
    for (auto child : parent->GetChildren()) {
        if (child->canUpdate)
            child->Update(delta);
        Update_(child, delta);
    }
}
