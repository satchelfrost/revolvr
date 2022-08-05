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
#include "spinning_pointer.h"

class RVRSceneTree {
public:
    RVRSceneTree();
    ~RVRSceneTree();
    int NewId();
    void CascadePose(const TrackedSpaceLocations& trackedSpaceLocations);
    void Update(float delta);
    std::vector<RVRMesh*> GatherRenderables();

private:
    void CascadePose_(RVRObject* parent, const TrackedSpaceLocations& trackedSpaceLocations);
    void GatherRenderables_(RVRObject* parent, std::vector<RVRMesh*>& renderables);
    void Update_(RVRObject* parent, float delta);
    RVRSpatial* root_;
    int nodeId_;
};