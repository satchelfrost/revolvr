#include "include/rvr_auto_scene_tree.h"

RVRAutoSceneTree::RVRAutoSceneTree() {
    Parser parser("example.rvr");
    auto units = parser.Parse();
    for (auto& unit : units) {
        RVRObject* object = ConstructTypeFromUnit(unit);
        objectMap_[object->id] = object;
    }
}

RVRObject* RVRAutoSceneTree::ConstructTypeFromUnit(Parser::Unit& unit) {
    int id = unit.heading.strKeyNumVal["id"];
    std::string typeName = unit.heading.strKeyStrVal["type"];
    if (typeName == "Mesh") {
        return new RVRMesh(id);
    }
    else if (typeName == "Origin") {
        return new RVROrigin(id);
    }
    else if (typeName == "Hand") {
        for (auto& field : unit.fields) {
            if (field.type == Parser::Hand) {
                return new RVRHand(id, field.handSide);
            }
        }
        THROW(Fmt("Failed to construct type \"Hand\", Field type or hand side not specified."));
    }
    else if (typeName == "Spatial") {
        return new RVRSpatial(id);
    }
    else {
        // Check for custom type
        for (auto& field : unit.fields) {
            if (field.type == Parser::Field1 && field.fieldName == "custom_type") {
                if (field.boolean) {
                    return ConstructCustomTypeFromUnit(unit);
                }
            }
        }
    }
    THROW(Fmt("Failed to construct type from string name %s", typeName.c_str()));
}

RVRObject* RVRAutoSceneTree::ConstructCustomTypeFromUnit(Parser::Unit& unit) {
   int id = unit.heading.strKeyNumVal["id"];
   std::string typeName = unit.heading.strKeyStrVal["type"];
   if (typeName == "SpinningPointer")
       return new SpinningPointer(id);
   THROW(Fmt("Failed to construct custom type from string name %s", typeName.c_str()));
}


RVRAutoSceneTree::~RVRAutoSceneTree() {
    root_->Destroy();
}

int RVRAutoSceneTree::NewId() {
    return nodeId_++;
}

void RVRAutoSceneTree::CascadePose(const TrackedSpaceLocations& trackedSpaceLocations) {
    CascadePose_(root_, trackedSpaceLocations);
}

void RVRAutoSceneTree::CascadePose_(RVRObject* parent, const TrackedSpaceLocations& trackedSpaceLocations) {
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

std::vector<RVRMesh*> RVRAutoSceneTree::GatherRenderables() {
    std::vector<RVRMesh*> renderables;
    GatherRenderables_(root_, renderables);
    return renderables;
}

void RVRAutoSceneTree::GatherRenderables_(RVRObject* parent, std::vector<RVRMesh*>& renderables) {
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

void RVRAutoSceneTree::Update(float delta) {
   Update_(root_, delta);
}

void RVRAutoSceneTree::Update_(RVRObject* parent, float delta) {
    for (auto child : parent->GetChildren()) {
        if (child->canUpdate)
            child->Update(delta);
        Update_(child, delta);
    }
}
