#include <spawn_box.h>
#include <global_context.h>
#include <helper_macros.h>

SpawnBox::SpawnBox(rvr::type::EntityId id) : Ritual(id) {
    // e.g. spatial_ = GetComponent<rvr::Spatial>(id);
}

void SpawnBox::Begin() {
    // Implement your code here
}

void SpawnBox::Update(float delta) {
    // Implement your code here
}

void SpawnBox::OnTriggered(rvr::Collider* other) {
    // Implement your code here
}

