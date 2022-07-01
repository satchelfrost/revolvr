#pragma once

#include "rvr_object.h"
#include "rvr_spatial.h"
#include "rvr_hand.h"
#include "rvr_origin.h"
#include "rvr_mesh.h"
#include "rvr_type.h"
#include "xr_linear.h"
#include "xr_app_helpers.h"
#include "pch.h"

class RVRSceneTree {
public:
    RVRSceneTree();
    int NewId();
//    void CascadePose(RVRSpatial* node);
    void CascadePose(const TrackedSpaceLocations& trackedSpaceLocations);
    std::vector<RVRMesh*> GatherRenderables();
    RVRSpatial* leftHand;
    RVRSpatial* rightHand;
    RVRSpatial* vrOrigin;

private:
    void CascadePose_(RVRSpatial* node, const TrackedSpaceLocations& trackedSpaceLocations);
    void GatherRenderables_(RVRObject* node, std::vector<RVRMesh*>& renderables);
    RVRSpatial* root_;
    int nodeId_;
};