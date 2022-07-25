#pragma once

#include "include/objects/rvr_object.h"
#include "include/objects/rvr_spatial.h"
#include "include/objects/rvr_hand.h"
#include "include/objects/rvr_origin.h"
#include "include/objects/rvr_mesh.h"
#include "rvr_type.h"
#include "include/math/xr_linear.h"
#include "xr_app_helpers.h"
#include "pch.h"
#include "../../fts-game/include/spinning_pointer.h"

class RVRSceneTree {
public:
    RVRSceneTree();
    int NewId();
    void CascadePose(const TrackedSpaceLocations& trackedSpaceLocations);
    void Update(float delta);
    std::vector<RVRMesh*> GatherRenderables();
    RVRSpatial* leftHand;
    RVRSpatial* rightHand;
    RVRSpatial* vrOrigin;

private:
    void CascadePose_(RVRSpatial* parent, const TrackedSpaceLocations& trackedSpaceLocations);
    void GatherRenderables_(RVRObject* parent, std::vector<RVRMesh*>& renderables);
    void Update_(RVRSpatial* parent, float delta);
    RVRSpatial* root_;
    int nodeId_;
};