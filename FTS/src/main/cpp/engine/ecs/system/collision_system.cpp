#include <ecs/system/collision_system.h>
#include <ecs/ecs.h>
#include <include/ecs/component/all_components.h>

namespace rvr {
void CollisionSystem::RunCollisionChecks() {
    auto components = ECS::Instance()->GetComponents(ComponentType::Collider);
//    for (auto itr1 = components.begin(); itr1 != components.end(); ++itr1) {
//        for (auto itr2 = itr1; ++itr2 != components.end();) {

    for(auto iter1 = components.begin(); iter1 != components.end(); ++iter1) {
        for(auto iter2 = iter1; ++iter2 != components.end();) {
            auto collider1 = reinterpret_cast<Collider*>((*iter1).second);
            auto collider2 = reinterpret_cast<Collider*>((*iter2).second);
//            Log::Write(Log::Level::Info, Fmt("%s",ECS::Instance()->GetEntity(collider1->id)->GetName().c_str()));
//            Log::Write(Log::Level::Info, Fmt("%s",ECS::Instance()->GetEntity(collider2->id)->GetName().c_str()));
            if (collider1->TestCollision(collider2)) {
                Log::Write(Log::Level::Info, "Collision");
            }
        }
    }
}
}
