#include <ecs/ecs.h>
#include <ecs/component/component_factory.h>
#include <ecs/component/all_components.h>
#include <all_ritual_types.h>
#include <include/ecs/component/types/colliders/sphere_collider.h>
#include <include/ecs/component/types/colliders/aabb_collider.h>

#define RITUAL_CASE(TYPE, NUM) case game::RitualType::TYPE: ritual = new TYPE(entity->id); break;

namespace rvr::componentFactory {
void CreateSpatial(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // Scale
    XrVector3f scale{};
    auto scaleField = fields.find("Spatial.scale");
    if (scaleField != fields.end()) {
        try {
            float x = scaleField->second.floatValues.at(0);
            float y = scaleField->second.floatValues.at(1);
            float z = scaleField->second.floatValues.at(2);
            scale = {x, y, z};
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of range, Spatial.scale was expecting 3 floats.",entity->GetName());
        }
    }

    // Position
    XrVector3f position{};
    auto posField = fields.find("Spatial.position");
    if (posField != fields.end()) {
        try {
            float x = posField->second.floatValues.at(0);
            float y = posField->second.floatValues.at(1);
            float z = posField->second.floatValues.at(2);
            position = {x, y, z};
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of range, Spatial.position was expecting 3 floats.",entity->GetName());
        }
    }

    // Orientation
    XrQuaternionf orientation{0, 0, 0, 1};
    auto oriField = fields.find("Spatial.orientation");
    if (oriField != fields.end()) {
        try {
            float x = oriField->second.floatValues.at(0);
            float y = oriField->second.floatValues.at(1);
            float z = oriField->second.floatValues.at(2);
            float w = oriField->second.floatValues.at(3);
            orientation = {x, y, z, w};
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of range, Spatial.quaternion was expecting 3 floats.",entity->GetName());
        }
    }

    // Create and assign spatial
    auto spatial = new Spatial(entity->id);
    ECS::Instance()->Assign(entity, spatial);
    spatial->pose.position = position;
    spatial->pose.orientation = orientation;
    spatial->scale = scale;
}

void CreateTrackedSpace(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // TrackedSpaceType - Required
    auto tsTypeField = fields.find("TrackedSpace.type");
    if (tsTypeField != fields.end()) {
        try {
            std::string trackedSpaceStr = tsTypeField->second.strValues.at(0);
            TrackedSpaceType trackedSpaceType = toTrackedSpaceTypeEnum(trackedSpaceStr);
            ECS::Instance()->Assign(entity, new TrackedSpace(entity->id, trackedSpaceType));
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of bounds, TrackedSpace.type was expecting 1 string",entity->GetName());
        }
    }
    else {
        ENTITY_ERR("TrackedSpace.type unspecified, cannot construct tracked space",entity->GetName());
    }
}

void CreateMesh(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    ECS::Instance()->Assign(entity, new Mesh(entity->id));
}

void CreateRitual(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // RitualType - Required
    game::RitualType rType;
    auto ritualTypeField = fields.find("Ritual.type");
    if (ritualTypeField != fields.end()) {
        try {
            std::string ritualStr = ritualTypeField->second.strValues.at(0);
            rType = game::toRitualTypeEnum(ritualStr);
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of bounds, Ritual.type was expecting 1 string", entity->GetName());

        }
    }
    else {
        ENTITY_ERR("Ritual.type unspecified, cannot construct ritual",entity->GetName());
    }

    // Can update
    bool canUpdate = true;
    auto canUpdateField = fields.find("Ritual.can_update");
    if (canUpdateField != fields.end()) {
        try {
            std::string strBool = canUpdateField->second.strValues.at(0);
            canUpdate = (strBool == "false") ? false : true;
        }
        catch (std::out_of_range& e ) {
            ENTITY_ERR("Out of bounds, Collider.can_update not found",entity->GetName());
        }
    }

    // Construct and assign ritual
    Ritual* ritual;
    switch (rType) {
        RITUALS(RITUAL_CASE)
        default:
            ENTITY_ERR("Ritual.type unspecified, cannot construct ritual",entity->GetName());
    }
    ECS::Instance()->Assign(entity, ritual);
    ritual->canUpdate = canUpdate;
}

void CreateCollider(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // ColliderType - Required
    Collider::ColliderType colliderType;
    auto colliderTypeField = fields.find("Collider.type");
    if (colliderTypeField != fields.end()) {
        try {
            std::string colliderTypeStr = colliderTypeField->second.strValues.at(0);
            colliderType = Collider::StrToColliderTypeEnum(colliderTypeStr);
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of bounds, Collider.type not found",entity->GetName());
        }
    }
    else {
        ENTITY_ERR("Collider.type not found", entity->GetName());
    }


    // Radius - Sphere Collider
    float radius;
    auto radiusField = fields.find("Collider.radius");
    if (radiusField != fields.end()) {
        try {
            radius = radiusField->second.floatValues.at(0);
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of bounds, Collider.radius not found",entity->GetName());
        }
    }

    // half_x, half_y, half_z - AABB Collider
    float halfX, halfY, halfZ;
    auto halfXField = fields.find("Collider.half_x");
    auto halfYField = fields.find("Collider.half_y");
    auto halfZField = fields.find("Collider.half_z");
    if ((halfXField != fields.end()) || (halfYField != fields.end()) || (halfZField != fields.end())) {
        try {
            halfX = halfXField->second.floatValues.at(0);
            halfY = halfYField->second.floatValues.at(0);
            halfZ = halfZField->second.floatValues.at(0);
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of bounds, collider half extent not found",entity->GetName());
        }

    }

    // Construct and assign collider
    if (colliderType == Collider::ColliderType::Sphere) {
        if (radiusField == fields.end())
            ENTITY_ERR("Sphere collider requires radius",entity->GetName());
        ECS::Instance()->Assign(entity, new SphereCollider(entity->id, radius));
    }
    else if (colliderType == Collider::ColliderType::AABB) {
        if ((halfXField == fields.end()) || (halfYField == fields.end()) || (halfZField == fields.end()))
            ENTITY_ERR("AABB collider requires half extents x, y, & z",entity->GetName());
        ECS::Instance()->Assign(entity, new AABBCollider(entity->id, halfX, halfY, halfZ));
    }
    else {
        Log::Write(Log::Level::Warning,
                Fmt("[entity: %s] - Could not construct collider %s, no implementation",
                        entity->GetName().c_str(),
                        Collider::ColliderTypeToString(colliderType).c_str()));
    }
}
}