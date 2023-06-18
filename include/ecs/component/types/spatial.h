#pragma once

#include <ecs/component/component.h>
#include <math/transform.h>

namespace rvr {
class Spatial : public Component {
public:

    Spatial(type::EntityId pId);
    Spatial(type::EntityId pId, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);
    Spatial(type::EntityId pId, math::Transform local, math::Transform world);

    Spatial(const Spatial& other) = delete;
    Spatial(const Spatial& other, type::EntityId pId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    math::Transform GetLocal();
    void SetLocal(const math::Transform& value);
    void SetLocalPose(const math::Pose& pose);
    void SetLocalScale(const glm::vec3& scale);
    void SetLocalPosition(const glm::vec3& position);
    void SetLocalOrientation(const glm::quat& orientation);

    math::Transform GetWorld();
    void SetWorld(const math::Transform& value);
    void SetWorldPose(const math::Pose& pose);
    void SetWorldScale(const glm::vec3& scale);
    void SetWorldPosition(const glm::vec3& position);
    void SetWorldOrientation(const glm::quat& orientation);

    // Get player relative transform
    math::Transform GetPlart();
    void SetPlart(const math::Transform& value);

    void UpdateWorld();

private:

    void ApplyParentRTS(math::Transform parent);
    void CalculatePlart();

    math::Transform local;
    math::Transform world;
    math::Transform plart; // player relative transform
};
}