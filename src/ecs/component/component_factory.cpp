/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/component/component_factory.h>
#include <ecs/component/component_hdrs.h>
#include <ritual_list.h>
#include <ritual_hdrs.h>
#include <ritual_type.h>
#include <audio/wav_audio_source.h>

// TODO: Can these be moved to component_hdrs.h
#include <ecs/component/types/colliders/sphere_collider.h>
#include <ecs/component/types/colliders/aabb_collider.h>
#include <ecs/component/types/audio/spatial_audio.h>

#include <global_context.h>
#include <math/linear_math.h>
#include <ecs/component/component_factory_util.h>

#define Assign GlobalContext::Inst()->GetECS()->Assign

namespace rvr::componentFactory {
void CreateSpatial(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    glm::vec3 scale{1, 1 ,1};
    float x, y, z;
    if (GetFloat3Field(entity, fields, "Spatial.scale", x, y, z))
        scale = {x, y, z};

    glm::vec3 position{0, 0, 0};
    if (GetFloat3Field(entity, fields, "Spatial.position", x, y, z))
        position = {x, y, z};

    glm::quat orientation{1,0,0,0};
    if (GetFloat3Field(entity, fields, "Spatial.euler", x, y, z))
        orientation = math::quaternion::FromEuler(x, y, z);

    Assign(entity, new Spatial(entity->id, position, orientation, scale));
}

void CreateTrackedSpace(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    std::string trackedSpaceStr;
    if (GetStringField(entity, fields, "TrackedSpace.type", trackedSpaceStr)) {
        TrackedSpaceType trackedSpaceType = toTrackedSpaceTypeEnum(trackedSpaceStr);
        if (trackedSpaceType == TrackedSpaceType::Head)
            GlobalContext::Inst()->GetAudioEngine()->SetHeadId(entity->id);
        Assign(entity, new TrackedSpace(entity->id, trackedSpaceType));
    }
    else {
        ENTITY_ERR("TrackedSpace.type unspecified, cannot construct tracked space", entity->GetName());
    }
}

void CreateMesh(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    bool visible = true;
    GetBoolField(entity, fields, "Mesh.visible", visible);
    Mesh* mesh = new Mesh(entity->id, visible);
    std::string name;
    if (GetStringField(entity, fields, "Mesh.gltf", name))
        mesh->SetName(name);
    Assign(entity, mesh);
}

void CreateRitual(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    RitualType rType;
    std::string ritualTypeStr;
    if (GetStringField(entity, fields, "Ritual.type", ritualTypeStr))
        rType = toRitualTypeEnum(ritualTypeStr);
    else
        ENTITY_ERR("Ritual.type unspecified, cannot construct ritual",entity->GetName());

    bool canUpdate = true;
    GetBoolField(entity, fields, "Ritual.can_update", canUpdate);

    Ritual* ritual;
    switch (rType) {
        #define RITUAL_CASE(TYPE) case RitualType::TYPE: ritual = new TYPE(entity->id); break;
        RITUAL_LIST(RITUAL_CASE)
        #undef RITUAL_CASE
        default:
            ENTITY_ERR("Ritual.type unspecified, cannot construct ritual", entity->GetName());
    }

    ritual->canUpdate = canUpdate;
    Assign(entity, ritual);
}

void CreateCollider(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    Collider::ColliderType colliderType;
    std::string colliderTypeStr;
    if (GetStringField(entity, fields, "Collider.type", colliderTypeStr))
        colliderType = Collider::StrToColliderTypeEnum(colliderTypeStr);
    else
        ENTITY_ERR("Collider.type not found", entity->GetName());

    float radius, halfX, halfY, halfZ = 0.05;
    switch (colliderType) {
    case Collider::ColliderType::Sphere:
        GetFloatField(entity, fields, "Collider.radius", radius);
        Assign(entity, new SphereCollider(entity->id, radius));
        break;
    case Collider::ColliderType::AABB:
        GetFloat3Field(entity, fields, "Collider.half_extents", halfX, halfY, halfZ);
        Assign(entity, new AABBCollider(entity->id, halfX, halfY, halfZ));
        break;
    case Collider::ColliderType::OBB:
        Log::Write(Log::Level::Warning,
                   Fmt("[entity: %s] - Could not construct collider %s, no implementation",
                       entity->GetName().c_str(),
                       Collider::ColliderTypeToString(colliderType).c_str()));
    }
}

void CreateAudio(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    std::string trackName;
    if (!GetStringField(entity, fields, "Audio.track_name", trackName))
        ENTITY_ERR("Audio.track_name was not specified", entity->GetName());

    float volume = 1.0;
    GetFloatField(entity, fields, "Audio.volume", volume);

    bool loop = false;
    GetBoolField(entity, fields, "Audio.loop", loop);

    bool spatialize = false;
    GetBoolField(entity, fields, "Audio.spatialize", spatialize);

    Audio* audio;
    if (spatialize)  {
        float spatialId = -1.0;
        GetFloatField(entity, fields, "Audio.spatial_id", spatialId);
        if (spatialId == -1.0)
            ENTITY_ERR("Using spatial audio, must specify Audio.spatial_id",entity->GetName());

        auto wav = WavAudioSource(trackName, false);
        audio = new SpatialAudio(entity->id, wav, (int)spatialId);
    }
    else {
        auto wav = WavAudioSource(trackName, true);
        audio = new Audio(entity->id, wav);
    }

    audio->Loop(loop);
    audio->volume = volume;
    Assign(entity, audio);
}

void CreateTimer(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    bool autoStart = false;
    GetBoolField(entity, fields, "Timer.auto_start", autoStart);

    bool oneShot = true;
    GetBoolField(entity, fields, "Timer.one_shot", oneShot);

    float waitTime;
    if(GetFloatField(entity, fields, "Timer.wait_time_seconds", waitTime)) {
        auto duration = std::chrono::seconds((int)waitTime);
        Assign(entity, new Timer(entity->id, autoStart, oneShot, duration));
    }
    else if (GetFloatField(entity, fields, "Timer.wait_time_milliseconds", waitTime)) {
        auto duration = std::chrono::milliseconds((int)waitTime);
        Assign(entity, new Timer(entity->id, autoStart, oneShot, duration));
    }
    else {
        int defaultWaitTime = 1;
        auto duration = std::chrono::seconds(defaultWaitTime);
        Log::Write(Log::Level::Warning,
                   Fmt("Timer wait time not set, using default of %d seconds", defaultWaitTime));
        Assign(entity, new Timer(entity->id, autoStart, oneShot, duration));
    }
}

void CreatePointLight(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    std::string colorName;
    float r, g, b;
    glm::vec3 color{0.0f, 0.0f, 0.0f};
    if (GetFloat3Field(entity, fields, "PointLight.color_norm_rgb", r, g, b))
        color = {r, g, b};
    if (GetFloat3Field(entity, fields, "PointLight.color_u8_rgb", r, g, b))
        color = {r / 255.0f, g / 255.0f, b /  255.0f};
    if (GetStringField(entity, fields, "PointLight.color_name", colorName))
        color =  GetNormalizedRGBFromColorName(colorName);

    float intensity = 0.4f; // hopefully a sane default
    GetFloatField(entity, fields, "PointLight.intensity", intensity);
    Assign(entity, new PointLight(entity->id, color, intensity));
}

void CreatePointCloud(Entity *entity, const std::map<std::string, Parser::Field>& fields) {
    std::string name;
    PointCloud* pointCloud;
    if (GetStringField(entity, fields, "PointCloud.ply", name)) {
        pointCloud = new PointCloud(entity->id, name, PointCloud::Ply);
    } else if (GetStringField(entity, fields, "PointCloud.las", name)) {
        pointCloud = new PointCloud(entity->id, name, PointCloud::Las);
    } else if (GetStringField(entity, fields, "PointCloud.laz", name)) {
        pointCloud = new PointCloud(entity->id, name, PointCloud::Laz);
    } else if (GetStringField(entity, fields, "PointCloud.e57", name)) {
        pointCloud = new PointCloud(entity->id, name, PointCloud::E57);
    } else  {
        ENTITY_ERR("PointCloud must specify PointCloud.ply|las|laz|e57",entity->GetName());
    }

    Assign(entity, pointCloud);
}
}