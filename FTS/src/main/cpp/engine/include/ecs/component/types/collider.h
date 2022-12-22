#pragma once

#include <ecs/component/component.h>


namespace rvr {
class Collider : public Component {
public:
    enum class ColliderType {
        Sphere,
        AABB,
        OBB
    };

    Collider(ColliderType pType, type::EntityId pId);

    static std::string ColliderTypeToString(ColliderType type);
    static ColliderType StrToColliderTypeEnum(const std::string& str);

    virtual bool TestSphere(Collider* collider) = 0;
    virtual bool TestAABB(Collider* collider) = 0;
    virtual bool TestOBB(Collider* collider) = 0;

    const ColliderType type;
private:
    static const int colliders = 3;
};
}
