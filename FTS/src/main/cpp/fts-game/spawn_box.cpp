#include <spawn_box.h>
#include <global_context.h>
#include <helper_macros.h>
#include <action/io.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/mesh.h>

SpawnBox::SpawnBox(rvr::type::EntityId id) : Ritual(id), offset_(0) {}

void SpawnBox::Update(float delta) {
    if (ButtonPressed(rvr::ActionType::A))
        ManualApproach();

    if (ButtonPressed(rvr::ActionType::B))
        CloneApproach();

    if (ButtonPressed(rvr::ActionType::X)) {
        Log::Write(Log::Level::Info, "Destroying entities");
        for (auto entity : spawnedEntities_) {
            entity->Destroy();
            offset_ = 0;
        }
        spawnedEntities_.clear();
    }
}

rvr::Entity* SpawnBox::CreateBoxManually() {
    auto entity = CreateNewEntity();
    Assign(entity, new rvr::Spatial(entity->id));
    Assign(entity, new rvr::Mesh(entity->id));
    return entity;
}

rvr::Entity* SpawnBox::CreateBoxViaClone() {
    auto box = GetEntity(boxToClonesId);
    auto cloneBox = box->Clone();
    auto mesh = GetComponent<rvr::Mesh>(cloneBox->id);
    mesh->visible = true;
    return cloneBox;
}

void SpawnBox::AdjustBoxPosition(rvr::Entity *entity) {
    auto spatial = GetComponent<rvr::Spatial>(entity->id);
    float fOffset = -1.0f * (float)offset_;
    offset_++;
    glm::vec3 pos = spatial->GetLocal().GetPosition();
    pos.z = fOffset;
    spatial->SetLocalPosition(pos);
}

void SpawnBox::ManualApproach() {
    auto parent = CreateBoxManually();
    spawnedEntities_.push_back(parent);
    AdjustBoxPosition(parent);

    // Add a child to the parent entity to test that recursive destroy is working
    auto childEntity = CreateBoxManually();
    auto childSpatial = GetComponent<rvr::Spatial>(childEntity->id);
    childSpatial->SetLocalScale({0.2, 0.2, 0.2});
    childSpatial->SetLocalPosition({0, 1, 0});
    parent->AddChild(childEntity);

    Log::Write(Log::Level::Info, Fmt("Manual entity %d was added with child %d",
                                     parent->id, childEntity->id));
}

void SpawnBox::CloneApproach() {
    auto entity = CreateBoxViaClone();
    AdjustBoxPosition(entity);
    spawnedEntities_.push_back(entity);
    Log::Write(Log::Level::Info, Fmt("Clone entity %d was added", entity->id));
}