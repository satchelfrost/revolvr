/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <ecs/system/collision_system.h>
#include <ecs/component/types/collider.h>
#include <global_context.h>

namespace rvr::system::collision {
void RunCollisionChecks() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Collider);
    for(auto iter1 = components.begin(); iter1 != components.end(); ++iter1) {
        for(auto iter2 = iter1; ++iter2 != components.end();) {
            auto collider1 = reinterpret_cast<Collider*>((*iter1).second);
            auto collider2 = reinterpret_cast<Collider*>((*iter2).second);
            if (collider1->TestCollision(collider2)) {
                collider1->Collided(collider2);
                collider2->Collided(collider1);
            }
        }
    }
}
}
