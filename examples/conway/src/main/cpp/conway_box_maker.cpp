#include "conway_box_maker.h"
#include <global_context.h>
#include "helper_macros.h"
#include <ecs/component/types/mesh.h>
#include <ecs/component/types/spatial.h>

ConwayBoxMaker::ConwayBoxMaker(rvr::type::EntityId id) : Ritual(id), conwayEngine_(19, 19) {
    CLONE_BOX_ID = 5;
    auto spatial = GetComponent<rvr::Spatial>(CLONE_BOX_ID);
    boxHalfExtent_ = spatial->GetLocal().GetScale().x;

    // Create a glider and set the board
    std::vector<std::string> glider {
        "001",
        "101",
        "011"
    };
    conwayEngine_.SetInitialState(0, 0, glider);
}

void ConwayBoxMaker::OnTimeout() {
    // First destroy old board state. This is inefficient, but just a test.
    for (auto entity : entities_)
        entity->Destroy();
    entities_.clear();

    // Create entities
    for (int i = 0; i < conwayEngine_.height; i++) {
        for (int j = 0; j < conwayEngine_.width; j++) {
            if (conwayEngine_.GetLife(i, j)) {
                auto entity = CreateBoxViaClone();
                entities_.push_back(entity);
                AdjustBoxPosition(entity, i, j);
            }
        }
    }

    // Process new board state
    conwayEngine_.Process();
}

rvr::Entity *ConwayBoxMaker::CreateBoxViaClone() const {
    auto prototypeBox = GetEntity(CLONE_BOX_ID);
    auto box = prototypeBox->Clone();
    auto mesh = GetComponent<rvr::Mesh>(box->id);
    mesh->SetVisibilityRecursive(true);
    return box;
}

void ConwayBoxMaker::AdjustBoxPosition(rvr::Entity *entity, int yOffset, int xOffset) const {
    auto spatial = GetComponent<rvr::Spatial>(entity->id);
    glm::vec3 pos = spatial->GetLocal().GetPosition();
    pos.x += (boxHalfExtent_ * (float)xOffset);
    pos.z += (boxHalfExtent_ * (float)yOffset);
    spatial->SetLocalPosition(pos);
}
