/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include <ecs/component/component.h>
#include <ecs/component/types/ritual.h>

namespace rvr {
class Collider : public Component {
public:
    enum class ColliderType {
        Sphere,
        AABB,
        OBB
    };

    Collider(const Collider& other) = delete;
    static std::string ColliderTypeToString(ColliderType type);
    static ColliderType StrToColliderTypeEnum(const std::string& str);
    Collider(ColliderType pType, type::EntityId pId);
    virtual bool TestCollision(Collider* other) = 0;
    void Collided(Collider* other);

    const ColliderType type;
private:
    static const int colliders = 3;
};
}
