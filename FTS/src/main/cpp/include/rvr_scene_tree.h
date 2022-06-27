#pragma once

#include "rvr_object.h"
#include "my_rvr_spatial.h"
#include "my_rvr_mesh.h"
#include "rvr_type.h"
#include "xr_linear.h"
#include "pch.h"

class RVRSceneTree {
public:
    RVRSceneTree();
    int NewId();
    RVRObject* hands[2];
    void CascadePose(RVRSpatial* node);
    std::vector<RVRMesh*> GatherRenderables();

private:
    RVRObject* root_;
    int nodeId_;
//    RVRObject* vrOrigin_;
//    RVRObject* next_;
};