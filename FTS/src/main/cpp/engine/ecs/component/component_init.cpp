#include <ecs/ecs.h>
#include <ecs/component/component_init.h>
#include <ecs/component/all_components.h>

namespace rvr::componentInit {
void InitSpatial(Entity *entity, const Parser::Field& field) {
    // Allow default init
    if (field.fullyQualifiedName == "Spatial")
        return;

    auto spatial = ECS::Instance()->GetComponent<Spatial>(entity->id);
    if (field.fullyQualifiedName == "Spatial.scale") {
        if (field.floatValues.size() != 3)
            THROW(Fmt("[entity: %s] - Scale was expecting a vector of 3 floats.", entity->GetName().c_str()));
        spatial->scale = XrVector3f{field.floatValues[0], field.floatValues[1], field.floatValues[2]};

    }
    else if (field.fullyQualifiedName == "Spatial.position") {
        if (field.floatValues.size() != 3)
            THROW(Fmt("[entity: %s] - Position was expecting a vector of 3 floats.", entity->GetName().c_str()));
        spatial->pose.position = XrVector3f{field.floatValues[0], field.floatValues[1], field.floatValues[2]};
    }
    else if (field.fullyQualifiedName == "Spatial.orientation") {
        if (field.floatValues.size() != 4)
            THROW(Fmt("[entity: %s] - Orientation was expecting a vector of 4 floats.", entity->GetName().c_str()));
        spatial->pose.orientation = XrQuaternionf{field.floatValues[0],
                                                  field.floatValues[1],
                                                  field.floatValues[2],
                                                  field.floatValues[3]};
    }
    else {
        THROW(Fmt("[entity: %s] - No interface exists for %s.",
                  entity->GetName().c_str(),
                  field.fullyQualifiedName.c_str()));
    }
}

void InitTrackedSpace(Entity *entity, const Parser::Field& field) {
    auto trackedSpace = ECS::Instance()->GetComponent<TrackedSpace>(entity->id);
    if (field.fullyQualifiedName == "TrackedSpace.type") {
        if (field.strValues.size() != 1)
            THROW(Fmt("[entity: %s] - Tracked space was expecting 1 string", entity->GetName().c_str()));
        trackedSpace->type = toTrackedSpaceTypeEnum(field.strValues[0]);
    }
    else {
        THROW(Fmt("[entity: %s] - No interface exists for %s.",
                  entity->GetName().c_str(),
                  field.fullyQualifiedName.c_str()));
    }
}

void InitMesh(Entity *entity, const Parser::Field& field) {
    // Allow default init
    if (field.fullyQualifiedName == "Mesh")
        return;
}

void InitRitual(Entity *entity, const Parser::Field& field) {
    auto ritual = ECS::Instance()->GetComponent<Ritual>(entity->id);
    if (field.fullyQualifiedName == "Ritual.name") {
        if (field.strValues.size() != 1)
            THROW(Fmt("[entity: %s] - Ritual.name was expecting 1 string", entity->GetName().c_str()));
        ritual->ritualName = field.fullyQualifiedName[0];
    }
    else if (field.fullyQualifiedName == "Ritual.can_update") {
        if (field.strValues.size() != 1)
            THROW(Fmt("[entity: %s] - Ritual.can_update was expecting 1 string", entity->GetName().c_str()));
        std::string strBool = field.strValues[0];
        if (strBool == "true") {
            ritual->canUpdate = true;
        }
        else if (strBool == "false") {
            ritual->canUpdate = false;
        }
        else {
            THROW(Fmt("[entity: %s] - Ritual.can_update was expecting true or false", entity->GetName().c_str()));
        }

    }
    else {
        THROW(Fmt("[entity: %s] - No interface exists for %s.",
                  entity->GetName().c_str(),
                  field.fullyQualifiedName.c_str()));
    }
}
}