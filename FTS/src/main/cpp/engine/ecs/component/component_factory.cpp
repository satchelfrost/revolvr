#include <ecs/component/component_factory.h>
#include <ecs/component/all_components.h>
#include <all_ritual_types.h>
#include <audio/wav_audio_source.h>
#include <ecs/component/types/colliders/sphere_collider.h>
#include <ecs/component/types/colliders/aabb_collider.h>
#include <ecs/component/types/audio/spatial_audio.h>
#include <global_context.h>
#include <math/linear_math.h>

#define Assign GlobalContext::Inst()->GetECS()->Assign
#define RITUAL_CASE(TYPE, NUM) case game::RitualType::TYPE: ritual = new TYPE(entity->id); break;

namespace rvr::componentFactory {
void CreateSpatial(Entity *entity, const std::map<std::string, Parser::Field>& fields) {

    // Scale
    glm::vec3 scale{1, 1 ,1};
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
    glm::vec3 position{0, 0, 0};
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

    // Euler based Orientation
    glm::quat orientation{1,0,0,0};
    auto eulerField = fields.find("Spatial.euler");
    if (eulerField != fields.end()) {
        try {
            float x = eulerField->second.floatValues.at(0);
            float y = eulerField->second.floatValues.at(1);
            float z = eulerField->second.floatValues.at(2);
            orientation = math::quaternion::FromEuler(x, y, z);
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of range, Spatial.euler was expecting 3 floats.",entity->GetName());
        }
    }

    // Create and assign spatial
    auto spatial = new Spatial(entity->id, position, orientation, scale);
    Assign(entity, spatial);
}

void CreateTrackedSpace(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // TrackedSpaceType - Required
    auto tsTypeField = fields.find("TrackedSpace.type");
    if (tsTypeField != fields.end()) {
        try {
            std::string trackedSpaceStr = tsTypeField->second.strValues.at(0);
            TrackedSpaceType trackedSpaceType = toTrackedSpaceTypeEnum(trackedSpaceStr);
            if (trackedSpaceType == TrackedSpaceType::Head)
                GlobalContext::Inst()->headEntityId = entity->id;
            if (trackedSpaceType == TrackedSpaceType::Player)
                GlobalContext::Inst()->playerEntityId = entity->id;
            Assign(entity, new TrackedSpace(entity->id, trackedSpaceType));
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
    Assign(entity, new Mesh(entity->id));
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
    Assign(entity, ritual);
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
        Assign(entity, new SphereCollider(entity->id, radius));
    }
    else if (colliderType == Collider::ColliderType::AABB) {
        if ((halfXField == fields.end()) || (halfYField == fields.end()) || (halfZField == fields.end()))
            ENTITY_ERR("AABB collider requires half extents x, y, & z",entity->GetName());
        Assign(entity, new AABBCollider(entity->id, halfX, halfY, halfZ));
    }
    else {
        Log::Write(Log::Level::Warning,
                Fmt("[entity: %s] - Could not construct collider %s, no implementation",
                        entity->GetName().c_str(),
                        Collider::ColliderTypeToString(colliderType).c_str()));
    }
}

void CreateAudio(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    // Track Name
    std::string trackName;
    auto nameField = fields.find("Audio.track_name");
    if (nameField != fields.end()) {
        try {
            trackName = nameField->second.strValues.at(0) + ".wav";
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of range, Audio.track_name was expecting a string",entity->GetName());
        }
    }

    // Volume
    float volume = 1.0;
    auto volumeField = fields.find("Audio.volume");
    if (volumeField != fields.end()) {
        try {
            volume = volumeField->second.floatValues.at(0);
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of range, Audio.volume was expecting a float",entity->GetName());
        }
    }

    // Loop
    bool loop = false;
    auto loopField = fields.find("Audio.loop");
    if (loopField != fields.end()) {
        try {
            std::string strBool = loopField->second.strValues.at(0);
            loop = (strBool == "false") ? false : true;
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of range, Audio.loop was expecting a boolean",entity->GetName());
        }
    }

    // spatialize
    bool spatialize = false;
    auto spatializeField = fields.find("Audio.spatialize");
    if (spatializeField!= fields.end()) {
        try {
            std::string strBool = spatializeField->second.strValues.at(0);
            spatialize = (strBool == "false") ? false : true;
        }
        catch (std::out_of_range& e) {
            ENTITY_ERR("Out of range, Audio.spatialize was expecting a boolean",entity->GetName());
        }
    }

    // Create and assign audio
    Audio* audio;
    if (spatialize) {
        auto wavAudioSource = new WavAudioSource(trackName.c_str(), false);
        audio = new SpatialAudio(entity->id, wavAudioSource);
    }
    else {
        auto wavAudioSource = new WavAudioSource(trackName.c_str(), true);
        audio = new Audio(entity->id, wavAudioSource);
    }
    Assign(entity, audio);
    audio->Loop(loop);
    audio->volume = volume;
}
}