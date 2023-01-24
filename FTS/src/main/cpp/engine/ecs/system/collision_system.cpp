#include <ecs/system/collision_system.h>
#include <ecs/ecs.h>
#include <ecs/component/types/collider.h>
#include <ecs/system/notification/collision_event.h>

namespace rvr {
void CollisionSystem::RunCollisionChecks() {
    auto components = ECS::Instance()->GetComponents(ComponentType::Collider);
    for(auto iter1 = components.begin(); iter1 != components.end(); ++iter1) {
        for(auto iter2 = iter1; ++iter2 != components.end();) {
            auto collider1 = reinterpret_cast<Collider*>((*iter1).second);
            auto collider2 = reinterpret_cast<Collider*>((*iter2).second);
            if (collider1->TestCollision(collider2)) {
                collider1->Notify(collider2);
                collider2->Notify(collider1);
            }
        }
    }
}
}
