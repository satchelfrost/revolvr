#include <ecs/ecs.h>
#include <ecs/component/component_factory.h>
#include <ecs/component/all_components.h>
#include <all_ritual_types.h>

#define RITUAL_CASE(TYPE, NUM) case game::RitualType::TYPE: ritual = new TYPE(entity->id); break;

namespace rvr::componentFactory {
void CreateSpatial(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // First check to see if component has already been created so we don't create it twice
    Spatial* spatial;
    if (entity->HasComponent(ComponentType::Spatial)) {
        spatial = ECS::Instance()->GetComponent<Spatial>(entity->id);
    }
    else {
        spatial = new Spatial();
        ECS::Instance()->Assign(entity, spatial);
    }

    for (const auto& [name, field] : fields) {
        if (name == "Spatial") {
            continue;
        }
        else if (name == "Spatial.scale") {
            auto fVals = field.floatValues;
            if (fVals.size() != 3)
                THROW(Fmt("[entity: %s] - Scale was expecting a vector of 3 floats.",
                                entity->GetName().c_str()));
            spatial->scale = XrVector3f{fVals[0], fVals[1], fVals[2]};
        }
        else if (name == "Spatial.position") {
            auto fVals = field.floatValues;
            if (fVals.size() != 3)
                THROW(Fmt("[entity: %s] - Position was expecting a vector of 3 floats.",
                                entity->GetName().c_str()));
            spatial->pose.position = XrVector3f{fVals[0], fVals[1], fVals[2]};
        }
        else if (name == "Spatial.orientation") {
            auto fVals = field.floatValues;
            if (fVals.size() != 4)
                THROW(Fmt("[entity: %s] - Orientation was expecting a vector of 4 floats.",
                                entity->GetName().c_str()));
            spatial->pose.orientation = XrQuaternionf{fVals[0],fVals[1],fVals[2],fVals[3]};
        }
        else {
            THROW(Fmt("[entity: %s] - No interface exists for %s.",
                            entity->GetName().c_str(),
                            name.c_str()));
        }
    }
}

void CreateTrackedSpace(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // First check to see if component has already been created so we don't create it twice
    TrackedSpace* trackedSpace;
    if (entity->HasComponent(ComponentType::TrackedSpace)) {
        trackedSpace = ECS::Instance()->GetComponent<TrackedSpace>(entity->id);
    }
    else {
        trackedSpace = new TrackedSpace();
        ECS::Instance()->Assign(entity, trackedSpace);
    }

    for (const auto& [name, field] : fields) {
        if (name == "TrackedSpace.type") {
            if (field.strValues.size() != 1)
                THROW(Fmt("[entity: %s] - Tracked space was expecting 1 string",
                                entity->GetName().c_str()));
            trackedSpace->type = toTrackedSpaceTypeEnum(field.strValues[0]);
        }
        else {
            THROW(Fmt("[entity: %s] - No interface exists for %s.",
                      entity->GetName().c_str(),
                      field.fullyQualifiedName.c_str()));
        }
    }
}

void CreateMesh(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // First check to see if component has already been created so we don't create it twice
    if (entity->HasComponent(ComponentType::Mesh))
        return;
    else
        ECS::Instance()->Assign(entity, new Mesh());
}

void CreateRitual(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // First check to see if component has already been created so we don't create it twice
    Ritual* ritual;
    if (entity->HasComponent(ComponentType::Ritual)) {
        ritual = ECS::Instance()->GetComponent<Ritual>(entity->id);
    }
    else {
        if (fields.find("Ritual.type") == fields.end())
            THROW(Fmt("[entity: %s] - Ritual.type unspecified, cannot construct ritual",
                      entity->GetName().c_str()));
        auto strVals = fields.at("Ritual.type").strValues;
        if (strVals.size() != 1)
            THROW(Fmt("[entity: %s] - Ritual.can_update was expecting 1 string",
                      entity->GetName().c_str()));
        game::RitualType rType = game::toRitualTypeEnum(strVals[0]);
        switch (rType) {
            RITUALS(RITUAL_CASE)
            default:
                THROW(Fmt("[entity: %s] - Ritual.type unspecified, cannot construct ritual",
                        entity->GetName().c_str()));
        }
        ECS::Instance()->Assign(entity, ritual);
    }

    for (const auto& [name, field] : fields) {
        if (name == "Ritual.type") {
            continue;
        }
        else if (name == "Ritual.can_update") {
            auto strVals = field.strValues;
            if (strVals.size() != 1)
                THROW(Fmt("[entity: %s] - Ritual.can_update was expecting 1 string",
                          entity->GetName().c_str()));
            std::string strBool = strVals[0];
            if (strBool == "true") {
                ritual->canUpdate = true;
            } else if (strBool == "false") {
                ritual->canUpdate = false;
            } else {
                THROW(Fmt("[entity: %s] - Ritual.can_update was expecting true or false",
                          entity->GetName().c_str()));
            }
        }
        else {
            THROW(Fmt("[entity: %s] - No interface exists for %s.",
                      entity->GetName().c_str(),
                      field.fullyQualifiedName.c_str()));
        }
    }
}
}