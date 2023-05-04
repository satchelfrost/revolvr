#include <spawn_box.h>
#include <global_context.h>
#include <helper_macros.h>
#include <action/io.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/mesh.h>

SpawnBox::SpawnBox(rvr::type::EntityId id) : Ritual(id), offset_(0) {
    // e.g. spatial_ = GetComponent<rvr::Spatial>(id);
}

void SpawnBox::Begin() {
    // Implement your code here
}

void SpawnBox::Update(float delta) {
    if (ButtonPressed(rvr::ActionType::A)) {
        auto entity = CreateBoxManually();
//        auto entity = CreateBoxViaClone();
        spawnedEntities_.push_back(entity);
        Log::Write(Log::Level::Info, Fmt("Entity %d was added", entity->id));
    }

    if (ButtonPressed(rvr::ActionType::B)) {
        Log::Write(Log::Level::Info, "Destroying entities");
        for (auto entity : spawnedEntities_) {
            entity->Destroy();
            offset_ = 0;
        }
        spawnedEntities_.clear();
    }
}

void SpawnBox::OnTriggered(rvr::Collider* other) {
    // Implement your code here
}

rvr::Entity* SpawnBox::CreateBoxManually() {
    auto entity = rvr::GlobalContext::Inst()->GetECS()->CreateNewEntity();
    glm::vec3 scale{1, 1 ,1};
    float fOffset = -1.0f * (float)offset_;
    offset_++;
    glm::vec3 position{0, 0, fOffset};
    glm::quat orientation{1,0,0,0};
    auto spatial = new rvr::Spatial(entity->id, position, orientation, scale);
    rvr::GlobalContext::Inst()->GetECS()->Assign(entity, spatial);
    rvr::GlobalContext::Inst()->GetECS()->Assign(entity, new rvr::Mesh(entity->id));

    // Parent the entity to the root
    auto parent = rvr::GlobalContext::Inst()->GetECS()->GetEntity(0);
    parent->AddChild(entity);

    return entity;
}

rvr::Entity* SpawnBox::CreateBoxViaClone() {
    auto box = rvr::GlobalContext::Inst()->GetECS()->GetEntity(10);
    auto cloneBox = box->Clone();
    return cloneBox;
}

