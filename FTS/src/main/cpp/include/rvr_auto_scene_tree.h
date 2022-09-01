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
#include "rvr_parser/parser.h"

class RVRAutoSceneTree {
public:
    RVRAutoSceneTree();
    ~RVRAutoSceneTree();
    int NewId();
    void CascadePose(const TrackedSpaceLocations& trackedSpaceLocations);
    void Update(float delta);
    std::vector<RVRMesh*> GatherRenderables();

private:
    void CascadePose_(RVRObject* parent, const TrackedSpaceLocations& trackedSpaceLocations);
    void GatherRenderables_(RVRObject* parent, std::vector<RVRMesh*>& renderables);
    void Update_(RVRObject* parent, float delta);
    RVRObject* ConstructTypeFromUnit(Parser::Unit& unit);
    RVRObject* ConstructCustomTypeFromUnit(Parser::Unit& unit);
    RVRSpatial* root_;
    int nodeId_;
    std::map<int, RVRObject*> objectMap_;
};
