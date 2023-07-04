/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <openxr/openxr.h>
#include <vector>
#include <math/transform.h>
#include <ecs/component/types/spatial.h>

namespace rvr {
class HandTracker {
public:
    enum class Hand {
        Left = 0,
        Right = 1,
        Count = 2,
        Both = 3
    };

private:
    Hand which_;

public:

    void Init(XrInstance instance, XrSession session, Hand which);
    ~HandTracker();
    void Update(XrTime predictedDisplayTime, XrSpace appSpace);
    bool IsPinching() const;
    void SetSpatialWithValidJointPose(int joint, Spatial* spatial);
    void EndSession();
    bool FullyHandTracked();

private:
    void SetupHandTracker(XrSession session);
    void SetupMeshes();
    void InitializeFunctionExtensions(XrInstance instance);
    void PrintHierarchy(std::vector<XrHandJointEXT> parentData);


    PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT_ = nullptr;
    PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT_ = nullptr;
    PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT_ = nullptr;
    PFN_xrGetHandMeshFB xrGetHandMeshFB_ = nullptr;

    XrHandTrackerEXT handTracker_ = XR_NULL_HANDLE;

    XrHandJointLocationEXT jointLocations_[XR_HAND_JOINT_COUNT_EXT];
    XrHandJointVelocityEXT jointVelocities_[XR_HAND_JOINT_COUNT_EXT];
    bool isPinching_;

};
}