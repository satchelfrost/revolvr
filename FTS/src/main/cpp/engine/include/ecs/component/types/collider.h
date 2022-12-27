#pragma once

#include <ecs/component/component.h>
#include <ecs/system/notification/subject.h>
#include <ecs/component/types/ritual.h>
#include <include/ecs/system/notification/collision_event.h>


namespace rvr {
class CollisionEvent;

class Collider : public Component {
public:
    enum class ColliderType {
        Sphere,
        AABB,
        OBB
    };
    static std::string ColliderTypeToString(ColliderType type);
    static ColliderType StrToColliderTypeEnum(const std::string& str);

    // Main interface
    Collider(ColliderType pType, type::EntityId pId);
    ~Collider();
    virtual bool TestCollision(Collider* other) = 0;

    // Subject interface
    void AddObserver(Ritual* ritual);
    void RemoveObserver(Ritual* ritual);
    void Notify(Collider* other);

    const ColliderType type;
private:
    static const int colliders = 3;
    Subject subject_;
    CollisionEvent* collisionEvent_;
};
}
