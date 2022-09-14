#pragma once

#include "pch.h"
#include "objects/rvr_object.h"
#include "objects/rvr_spatial.h"
#include "objects/rvr_hand.h"
#include "objects/rvr_origin.h"
#include "objects/rvr_mesh.h"
#include "objects/rvr_type.h"
#include "math/xr_linear.h"
#include "xr_app_helpers.h"
#include "../../fts-game/include/spinning_pointer.h"
#include "rvr_parser/parser.h"

class RVRAutoSceneTree {
public:
    RVRAutoSceneTree();
    ~RVRAutoSceneTree();

    void Load(const std::string& fileName);
    void CascadePose(const TrackedSpaceLocations& trackedSpaceLocations);
    void Update(float delta);
    std::vector<RVRMesh*> GatherRenderables();

private:
    void CascadePose_(RVRObject* parent, const TrackedSpaceLocations& trackedSpaceLocations);
    void GatherRenderables_(RVRObject* parent, std::vector<RVRMesh*>& renderables);
    void Update_(RVRObject* parent, float delta);
    RVRObject* ConstructTypeFromUnit(Parser::Unit& unit);
    static RVRObject* ConstructCustomTypeFromUnit(Parser::Unit& unit, int id);
    static void PopulateSpatialFromFields(RVRSpatial* spatial, std::vector<Parser::Field>& fields);
    RVRSpatial* root_;
    std::map<int, RVRObject*> objectMap_; // id -> object
    std::map<int, int> parentIdMap_;      // child_id -> parent_id
    constexpr static int rootId_ = 1;
    constexpr static int nullId_ = 0;
    bool rootIdFound_;
};
