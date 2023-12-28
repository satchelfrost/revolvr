#include "timer_box_maker.h"
#include "global_context.h"
#include "helper_macros.h"
#include "ecs/component/types/spatial.h"
#include "ecs/component/types/mesh.h"

TimerBoxMaker::TimerBoxMaker(rvr::type::EntityId id) : Ritual(id), offset_(0) {}

void TimerBoxMaker::OnTimeout() {
    if (spawnedEntities_.size() < 21) {
        auto entity = CreateBoxViaClone();
        AdjustBoxPosition(entity);
        spawnedEntities_.push_back(entity);
        Log::Write(Log::Level::Info, Fmt("Clone entity %d was added", entity->id));
    }
    else {
        Log::Write(Log::Level::Info, "Destroying entities");
        for (auto entity : spawnedEntities_) {
            entity->Destroy();
            offset_ = 0;
        }
        spawnedEntities_.clear();
    }
}

rvr::Entity* TimerBoxMaker::CreateBoxViaClone() const {
    int BOX_TO_CLONE = 5;
    auto prototypeBox = GetEntity(BOX_TO_CLONE);
    auto box = prototypeBox->Clone();
    auto mesh = GetComponent<rvr::Mesh>(box->id);
    mesh->SetVisibilityRecursive(true);
    return box;
}

void TimerBoxMaker::AdjustBoxPosition(rvr::Entity *entity) {
    auto spatial = GetComponent<rvr::Spatial>(entity->id);
    float fOffset = (float)offset_;
    offset_++;
    glm::vec3 pos = spatial->GetLocal().GetPosition();
    pos.x += fOffset;
    spatial->SetLocalPosition(pos);
}

