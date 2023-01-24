#include <ecs/system/notification/collision_event.h>

namespace rvr {
CollisionEvent::CollisionEvent(Collider* src, Collider* other) :
 Event(EventType::Collision), src(src), other(other) {}
}
