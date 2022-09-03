#include "include/rvr_auto_scene_tree.h"

RVRAutoSceneTree::RVRAutoSceneTree() : rootIdFound_(false) {
    Parser parser("example.rvr");
    auto units = parser.Parse();

    // load up units into the object map
    for (auto& unit : units) {
        RVRObject* object = ConstructTypeFromUnit(unit);
        objectMap_[object->id] = object;
    }

    // assign the root
    root_ = dynamic_cast<RVRSpatial*>(objectMap_[rootId_]);
    if (!root_)
        THROW(Fmt("Failed to construct root, node with id=1 was probably never specified."));

    // setup the tree
    for (auto& node : objectMap_) {
        // ignore root node
        if (node.first == rootId_)
            continue;

        RVRObject* child = node.second;
        int parentId = parentIdMap_[child->id];
        RVRObject* parent = objectMap_[parentId];
        if (!parent)
            THROW(Fmt("Could not find parent with id=%d for child %s",
                           parentId,
                           child->GetName().c_str()));
        parent->AddChild(child);
    }
}

RVRObject* RVRAutoSceneTree::ConstructTypeFromUnit(Parser::Unit& unit) {
    RVRObject* object;
    int id = unit.heading.strKeyNumVal["id"];
    if (id == 0)
        THROW(Fmt("id=0 not allowed"));

    if (id == 1) {
        if (!rootIdFound_)
            rootIdFound_ = true;
        else
            THROW(Fmt("id=1 can only be specified once for root node."));
    }

    std::string typeName = unit.heading.strKeyStrVal["type"];
    if (typeName == "Mesh") {
        object = new RVRMesh(id);
    }
    else if (typeName == "Origin") {
        object = new RVROrigin(id);
    }
    else if (typeName == "Hand") {
        for (auto& field : unit.fields) {
            if (field.type == Parser::Hand) {
                object = new RVRHand(id, field.handSide);
                goto finish;
            }
        }
        THROW(Fmt("Failed to construct type \"Hand\", Field type or hand side not specified."));
    }
    else if (typeName == "Spatial") {
        object = new RVRSpatial(id);
    }
    else {
        // Check for custom type
        for (auto& field : unit.fields) {
            if (field.type == Parser::Field1 && field.fieldName == "custom_type") {
                if (field.boolean) {
                    object = ConstructCustomTypeFromUnit(unit, id);
                    goto finish;
                }
            }
        }
        THROW(Fmt("Failed to construct type from string name %s", typeName.c_str()));
    }

finish:
    // Set the name
    std::string name = unit.heading.strKeyStrVal["name"];
    if (!name.empty())
        object->SetName(name);

    // Set the weak parent id
    int parentId = unit.heading.strKeyNumVal["parent"];
    if (id == rootId_)
        parentIdMap_[id] = nullId_;
    else if (parentId == nullId_)
        parentIdMap_[id] = rootId_;
    else
        parentIdMap_[id] = parentId;

    PopulateSpatialFromFields(dynamic_cast<RVRSpatial*>(object), unit.fields);
    return object;
}

RVRObject* RVRAutoSceneTree::ConstructCustomTypeFromUnit(Parser::Unit& unit, int id) {
   std::string typeName = unit.heading.strKeyStrVal["type"];
   if (typeName == "SpinningPointer")
       return new SpinningPointer(id);
   THROW(Fmt("Failed to construct custom type from string name %s", typeName.c_str()));
}

void RVRAutoSceneTree::PopulateSpatialFromFields(RVRSpatial* spatial,
                                                 std::vector<Parser::Field>& fields) {
    for (auto& field : fields) {
        switch (field.type) {
            case Parser::Field3: // scale and position
                if (field.fieldName == "position")
                    spatial->pose.position = field.vec3;
                else if (field.fieldName == "scale")
                    spatial->scale = field.vec3;
                else
                    THROW(Fmt("Field3 %s cannot be handled", field.fieldName.c_str()));
                break;
            case Parser::Field4: // orientation
                if (field.fieldName == "orientation")
                    spatial->pose.orientation = field.quat;
                else
                    THROW(Fmt("Field4 %s cannot be handled", field.fieldName.c_str()));
                break;
            case Parser::Field1: // visible, custom_type, can_update
                if (field.fieldName == "visible")
                    spatial->visible = field.boolean;
                else if (field.fieldName == "custom_type")
                    break;
                else if (field.fieldName == "can_update")
                    spatial->canUpdate = field.boolean;
                else
                    THROW(Fmt("Field1 %s cannot be handled", field.fieldName.c_str()));
                break;
            case Parser::Field2:
            case Parser::Resource:
            case Parser::Hand:
                break;
        }
    }
}


RVRAutoSceneTree::~RVRAutoSceneTree() {
    root_->Destroy();
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
                XrQuaternionf_Multiply(&childSpatial->worldPose.orientation,
                                       &childSpatial->pose.orientation,
                                       &parentSpatial->worldPose.orientation);
                XrVector3f offset = XrQuaternionf_Rotate(parentSpatial->worldPose.orientation,
                                                         childSpatial->pose.position);
                XrVector3f_Add(&childSpatial->worldPose.position, &offset,
                               &parentSpatial->worldPose.position);

                // Place objects relative to the origin
                if (parent->type == RVRType::Origin)
                    XrVector3f_Sub(&childSpatial->worldPose.position,
                                   &childSpatial->worldPose.position,
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
