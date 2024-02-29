#include <movement.h>
#include <global_context.h>
#include <helper_macros.h>
#include <io/io.h>
#include <stdio.h>
#define POINT_CLOUD_ID 3
#define ANCHOR_ID 2
#define SUPER_ANCHOR_ID 4

Movement::Movement(rvr::type::EntityId id) : Ritual(id) {}

void Movement::Begin() {}

void Movement::Update(float delta) {
    auto pc = GetEntity(POINT_CLOUD_ID);
    auto spatial = GetComponent<rvr::Spatial>(pc->id);
//    auto pos = spatial->GetLocal().GetPosition();
    auto anchor = GetEntity(ANCHOR_ID);
    auto anchorSpt = GetComponent<rvr::Spatial>(anchor->id);
    auto superAnchor = GetEntity(SUPER_ANCHOR_ID);
    auto superAnchorSpt = GetComponent<rvr::Spatial>(superAnchor->id);
    auto pos = superAnchorSpt->GetLocal().GetPosition();

    if (ButtonPressed(rvr::ActionType::A)) {
        char tmp[100];
        sprintf(tmp, "Position: {x, y, z} = {%.5f, %.5f, %.5f}", pos.x, pos.y, pos.z);
        rvr::PrintInfo(tmp);
        auto ori = anchorSpt->GetLocal().GetOrientation();
        sprintf(tmp, "Orientation: {x, y, z, w} = {%.5f, %.5f, %.5f, %.5f}", ori.x, ori.y, ori.z, ori.w);
        rvr::PrintInfo(tmp);
    }

    auto rightJoy = GetJoystickXY(rvr::Hand::Right);
    pos.z -= rightJoy.y * delta * 10.0f;
    pos.x += rightJoy.x * delta * 10.0f;
    superAnchorSpt->SetLocalPosition(pos);

    auto leftJoy = GetJoystickXY(rvr::Hand::Left);
//    float angle = leftJoy.x * delta;
    float scaleAmt = leftJoy.y * delta;
    auto scale = superAnchorSpt->GetLocal().GetScale();
    scale.x += scaleAmt;
    scale.y += scaleAmt;
    scale.z += scaleAmt;
    superAnchorSpt->SetLocalScale(scale);

//    auto transform = anchorSpt->GetLocal().Rotated({0, 1, 0}, angle);
//    anchorSpt->SetLocal(transform);
}

void Movement::OnTriggered(rvr::Collider* other) {}

void Movement::OnTimeout() {}

