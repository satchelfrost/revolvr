#include "moving_sound_box.h"
#include "common.h"
#include "ecs/ecs.h"
#include "ecs/component/types/spatial.h"
#include "global_context.h"
#include "io/io.h"
#include "helper_macros.h"

MovingSoundBox::MovingSoundBox(rvr::type::EntityId id) : Ritual(id) {
    spatial_ = GetComponent<rvr::Spatial>(id);
    audio_ = GetComponent<rvr::Audio>(id);
}

void MovingSoundBox::Update(float delta) {
    // Move the sound box
    auto leftJoy = GetJoystickXY(rvr::Hand::Left);
    auto rightJoy = GetJoystickXY(rvr::Hand::Right);
    auto pos = spatial_->GetLocal().GetPosition();
    float speed = 3.0f;
    pos.z -=  leftJoy.y * speed * delta;
    pos.x +=  leftJoy.x * speed * delta;
    pos.y += rightJoy.y * speed * delta;
    spatial_->SetLocalPosition(pos);

    // Play the laser spatial audio sound
    if (ButtonPressed(rvr::ActionType::A))
        audio_->Play();

    // Play the achievement jingle (regular audio)
    int ACHIEVEMENT_ENTITY_ID = 2;
    if (ButtonPressed(rvr::ActionType::B))
        GetComponent<rvr::Audio>(ACHIEVEMENT_ENTITY_ID)->Play();
}