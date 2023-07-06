/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <io/hand_tracking/hand_tracker.h>
#include <common.h>

namespace rvr {
void HandTracker::Init(XrInstance instance, XrSession session, Hand which) {
    which_ = which;
    InitializeFunctionExtensions(instance);
    SetupHandTracker(session);
    SetupMeshes();
}

void HandTracker::InitializeFunctionExtensions(XrInstance instance) {
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrCreateHandTrackerEXT",
                                      (PFN_xrVoidFunction*)(&xrCreateHandTrackerEXT_)));
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrDestroyHandTrackerEXT",
                                      (PFN_xrVoidFunction*)(&xrDestroyHandTrackerEXT_)));
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrLocateHandJointsEXT",
                                      (PFN_xrVoidFunction*)(&xrLocateHandJointsEXT_)));
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetHandMeshFB",
                                      (PFN_xrVoidFunction*)(&xrGetHandMeshFB_)));
}

void HandTracker::SetupHandTracker(XrSession session) {
    XrHandTrackerCreateInfoEXT createInfo{XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT};
    createInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;

    if (which_ == Hand::Left)
        createInfo.hand = XR_HAND_LEFT_EXT;
    else if (which_ == Hand::Right)
        createInfo.hand = XR_HAND_RIGHT_EXT;
    else
        THROW("Cannot initialize hand tracker as which hand was not specified");

    CHECK_XRCMD(xrCreateHandTrackerEXT_(session, &createInfo, &handTracker_));
}

void HandTracker::SetupMeshes() {
    CHECK_MSG(xrGetHandMeshFB_, "Failed to setup meshes because xrGetHandMeshFB_ was not loaded");
    XrHandTrackingMeshFB mesh{XR_TYPE_HAND_TRACKING_MESH_FB};
    mesh.next = nullptr;

    // Figure out the sizes for the mesh

    // Mesh - skeleton
    mesh.jointCapacityInput = 0;
    mesh.jointCountOutput = 0;
    mesh.jointBindPoses = nullptr;
    mesh.jointRadii = nullptr;
    mesh.jointParents = nullptr;

    // Mesh - vertex
    mesh.vertexCapacityInput = 0;
    mesh.vertexCountOutput = 0;
    mesh.vertexPositions = nullptr;
    mesh.vertexNormals = nullptr;
    mesh.vertexUVs = nullptr;
    mesh.vertexBlendIndices = nullptr;
    mesh.vertexBlendWeights = nullptr;

    // Mesh - index
    mesh.indexCapacityInput = 0;
    mesh.indexCountOutput = 0;
    mesh.indices = nullptr;

    // Get mesh sizes
    CHECK_XRCMD(xrGetHandMeshFB_(handTracker_, &mesh));

    // Update sizes
    mesh.jointCapacityInput = mesh.jointCountOutput;
    mesh.vertexCapacityInput = mesh.vertexCountOutput;
    mesh.indexCapacityInput = mesh.indexCountOutput;

    // Skeleton
    std::vector<XrPosef> jointBindLocations;
    std::vector<XrHandJointEXT> parentData;
    std::vector<float> jointRadii;
    jointBindLocations.resize(mesh.jointCountOutput);
    parentData.resize(mesh.jointCountOutput);
    jointRadii.resize(mesh.jointCountOutput);
    mesh.jointBindPoses = jointBindLocations.data();
    mesh.jointParents = parentData.data();
    mesh.jointRadii = jointRadii.data();

    // Vertex
    std::vector<XrVector3f> vertexPositions;
    std::vector<XrVector3f> vertexNormals;
    std::vector<XrVector2f> vertexUVs;
    std::vector<XrVector4sFB> vertexBlendIndices;
    std::vector<XrVector4f> vertexBlendWeights;
    vertexPositions.resize(mesh.vertexCountOutput);
    vertexNormals.resize(mesh.vertexCountOutput);
    vertexUVs.resize(mesh.vertexCountOutput);
    vertexBlendIndices.resize(mesh.vertexCountOutput);
    vertexBlendWeights.resize(mesh.vertexCountOutput);
    mesh.vertexPositions = vertexPositions.data();
    mesh.vertexNormals = vertexNormals.data();
    mesh.vertexUVs = vertexUVs.data();
    mesh.vertexBlendIndices = vertexBlendIndices.data();
    mesh.vertexBlendWeights = vertexBlendWeights.data();

    // Index
    std::vector<int16_t> indices;
    indices.resize(mesh.indexCountOutput);
    mesh.indices = indices.data();

    // Fill in the data
    XrHandTrackingCapsulesStateFB capsulesStateFb{XR_TYPE_HAND_TRACKING_CAPSULES_STATE_FB};
    capsulesStateFb.next = nullptr;
    mesh.next = &capsulesStateFb;
    CHECK_XRCMD(xrGetHandMeshFB_(handTracker_, &mesh));

    // TODO: Possibly initialize some renderable with this mesh data
    // Interesting data includes:
    // General vertex and normal data
    // Joint radii
    // Capsule information

//    PrintHierarchy(parentData);
}

void HandTracker::Update(XrTime predictedDisplayTime, XrSpace appSpace) {
    XrHandTrackingScaleFB scale{XR_TYPE_HAND_TRACKING_SCALE_FB};
    scale.next = nullptr;
    scale.sensorOutput = 1.0f;
    scale.currentOutput = 1.0f;
    scale.overrideValueInput = 1.00f;
    scale.overrideHandScale = XR_FALSE;
    XrHandTrackingCapsulesStateFB capsuleState{XR_TYPE_HAND_TRACKING_CAPSULES_STATE_FB};
    capsuleState.next = &scale;
    XrHandTrackingAimStateFB aimState{XR_TYPE_HAND_TRACKING_AIM_STATE_FB};
    aimState.next = &capsuleState;
    XrHandJointVelocitiesEXT velocities{XR_TYPE_HAND_JOINT_VELOCITIES_EXT};
    velocities.next = &aimState;
    velocities.jointCount = XR_HAND_JOINT_COUNT_EXT;
    velocities.jointVelocities = jointVelocities_;
    XrHandJointLocationsEXT locations{XR_TYPE_HAND_JOINT_LOCATIONS_EXT};
    locations.next = &velocities;
    locations.jointCount = XR_HAND_JOINT_COUNT_EXT;
    locations.jointLocations = jointLocations_;

    XrHandJointsLocateInfoEXT locateInfo{XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT};
    locateInfo.baseSpace = appSpace;
    locateInfo.time = predictedDisplayTime;
    CHECK_XRCMD(xrLocateHandJointsEXT_(handTracker_, &locateInfo, &locations));

    isPinching_ = (aimState.status & XR_HAND_TRACKING_AIM_INDEX_PINCHING_BIT_FB) != 0;
}

HandTracker::~HandTracker() {
    // unhook extensions
    xrCreateHandTrackerEXT_ = nullptr;
    xrDestroyHandTrackerEXT_ = nullptr;
    xrLocateHandJointsEXT_ = nullptr;
    xrGetHandMeshFB_ = nullptr;
}

void HandTracker::PrintHierarchy(std::vector<XrHandJointEXT> parentData) {
    // Print hierarchy
    Log::Write(Log::Level::Info,
               Fmt("%s hand hierarchy info", (which_ == Hand::Left) ? "Left" : "Right"));
    for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; ++i) {
        auto transform = math::Transform(jointLocations_[i].pose);
        Log::Write(Log::Level::Info,
                   Fmt("{ {%.6f, %.6f, %.6f},  {%.6f, %.6f, %.6f, %.6f} } // joint = %d, parent = %d",
                       transform.GetPosition().x,
                       transform.GetPosition().y,
                       transform.GetPosition().z,
                       transform.GetOrientation().x,
                       transform.GetOrientation().y,
                       transform.GetOrientation().z,
                       transform.GetOrientation().w,
                       i,
                       (int)parentData[i]));
    }
}

bool HandTracker::IsPinching() const {
    return isPinching_;
}

// Sets the spatial pose with requested joint pose if the location is valid
void HandTracker::SetSpatialWithValidJointPose(int joint, Spatial* spatial) {
    if (joint < 0 || joint > 25)
        return;

    XrSpaceLocationFlags flags = XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT;
    bool isValid = jointLocations_[joint].locationFlags & flags;
    if (isValid)
        spatial->SetLocal(math::Transform(jointLocations_[joint].pose, spatial->GetLocal().GetScale()));
}

void HandTracker::EndSession() {
    if (xrDestroyHandTrackerEXT_ && handTracker_ != XR_NULL_HANDLE) {
        CHECK_XRCMD(xrDestroyHandTrackerEXT_(handTracker_));
    }
}

bool HandTracker::FullyHandTracked() {
    XrSpaceLocationFlags flags = XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT;
    for (auto joint : jointLocations_) {
        bool isValid = joint.locationFlags & flags;
        if (!isValid)
            return false;
    }
    return true;
}
}