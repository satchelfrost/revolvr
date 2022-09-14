#pragma once

#include "pch.h"
#include "include/objects/rvr_object.h"
#include "include/objects/rvr_spatial.h"
#include "include/objects/rvr_hand.h"
#include "include/objects/rvr_origin.h"
#include "include/objects/rvr_mesh.h"
#include "include/objects/rvr_type.h"
#include "include/math/xr_linear.h"
#include "xr_app_helpers.h"
#include "../../fts-game/include/spinning_pointer.h"

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