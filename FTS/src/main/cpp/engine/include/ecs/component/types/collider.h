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
