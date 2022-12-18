#include <ecs/ecs.h>
#include <ecs/component/component_init.h>
#include <ecs/component/all_components.h>

namespace rvr::componentInit {
void InitSpatial(Entity *entity, Parser::Field field) {
    auto [name, sVals, fVals] = Parser::GetTailAccessInfo(field.access);

    // This is the case when we are simply initializing a component with default values
    if (sVals.empty() && fVals.empty())
        return;

    auto spatial = ECS::Instance()->GetComponent<Spatial>(entity->id);
    if (name == "scale") {
        if (fVals.size() != 3)
            THROW(Fmt("[entity: %s] - Scale was expecting a vector of 3 floats.", entity->GetName().c_str()));
        spatial->scale = XrVector3f{fVals[0], fVals[1], fVals[2]};
    }
    else if (name == "position") {
        if (fVals.size() != 3)
            THROW(Fmt("[entity: %s] - Position was expecting a vector of 3 floats.", entity->GetName().c_str()));
        spatial->pose.position = XrVector3f{fVals[0], fVals[1], fVals[2]};
    }
    else if (name == "orientation") {
        if (fVals.size() != 4)
            THROW(Fmt("[entity: %s] - Orientation was expecting a vector of 4 floats.", entity->GetName().c_str()));
        spatial->pose.orientation = XrQuaternionf{fVals[0], fVals[1], fVals[2], fVals[3]};
    }
    else {
        THROW(Fmt("[entity: %s] - No interface exists for component member %s.",
                  entity->GetName().c_str(),
                  name.c_str()));
    }
}

void InitTrackedSpace(Entity *entity, Parser::Field field) {
    auto [name, sVals, fVals] = Parser::GetTailAccessInfo(field.access);

    // This is the case when we are simply initializing a component with default values
    if (sVals.empty() && fVals.empty())
        return;

    auto trackedSpace = ECS::Instance()->GetComponent<TrackedSpace>(entity->id);
    if (name == "type") {
        if (sVals.size() != 1)
            THROW(Fmt("[entity: %s] - Tracked space was expecting 1 string", entity->GetName().c_str()));
        trackedSpace->type = toTrackedSpaceTypeEnum(sVals[0]);
    }
    else {
        THROW(Fmt("[entity: %s] - No interface exists for component member %s.",
                  entity->GetName().c_str(),
                  name.c_str()));
    }
}

void InitMesh(Entity *entity, Parser::Field field) {}

void InitRitual(Entity *entity, Parser::Field field) {
    auto [name, sVals, fVals] = Parser::GetTailAccessInfo(field.access);

    // This is the case when we are simply initializing a component with default values
    if (sVals.empty() && fVals.empty())
        return;

    auto ritual = ECS::Instance()->GetComponent<Ritual>(entity->id);

    // Attach concrete ritual if it does not already exist
    if (name == "name") {
        if (sVals.size() != 1)
            THROW(Fmt("[entity: %s] - Ritual.name was expecting 1 string", entity->GetName().c_str()));
        ritual->ritualName = sVals[0];
    }
    else if (name == "can_update") {
        if (sVals.size() != 1)
            THROW(Fmt("[entity: %s] - Ritual.can_update was expecting 1 string", entity->GetName().c_str()));
        std::string strBool = sVals[0];
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
        THROW(Fmt("[entity: %s] - No interface exists for component member %s.",
                  entity->GetName().c_str(),
                  name.c_str()));
    }
}
}