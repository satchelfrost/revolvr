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
    auto entity = rvr::GlobalContext::Inst()->GetECS()->CreateNewEntity();
    glm::vec3 scale{1, 1 ,1};
    auto spatial = new rvr::Spatial(entity->id);
    spatial->SetLocalScale(scale);
    rvr::GlobalContext::Inst()->GetECS()->Assign(entity, spatial);
    rvr::GlobalContext::Inst()->GetECS()->Assign(entity, new rvr::Mesh(entity->id));

    // Parent the entity to the root
    // Todo: Make this parenting
    auto parent = rvr::GlobalContext::Inst()->GetECS()->GetEntity(0);
    parent->AddChild(entity);

    return entity;
}

rvr::Entity* SpawnBox::CreateBoxViaClone() {
    auto box = rvr::GlobalContext::Inst()->GetECS()->GetEntity(boxToClonesId);
    auto cloneBox = box->Clone();
    auto parent = rvr::GlobalContext::Inst()->GetECS()->GetEntity(0);
    parent->AddChild(cloneBox);
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
    auto entity = CreateBoxManually();
    AdjustBoxPosition(entity);
    spawnedEntities_.push_back(entity);
    Log::Write(Log::Level::Info, Fmt("Manual entity %d was added", entity->id));

//    // Add a child to the entity
//    auto childEntity = CreateBoxManually();
//    auto childSpatial = GetComponent<rvr::Spatial>(childEntity->id);
//    childSpatial->SetLocalScale({0.2, 0.2, 0.2});
//    auto parentSpatial = GetComponent<rvr::Spatial>(entity->id);
//    auto newChildPos = parentSpatial->GetLocal().GetPosition();
//    newChildPos.y += 1;
//    childSpatial->SetLocalPosition(newChildPos);
//
//    entity->AddChild(childEntity);
//
//    Log::Write(Log::Level::Info, Fmt("Manual entity %d was added with child %d",
//                                     entity->id, childEntity->id));
}

void SpawnBox::CloneApproach() {
    auto entity = CreateBoxViaClone();
    AdjustBoxPosition(entity);
    spawnedEntities_.push_back(entity);
    Log::Write(Log::Level::Info, Fmt("Clone entity %d was added", entity->id));
}

