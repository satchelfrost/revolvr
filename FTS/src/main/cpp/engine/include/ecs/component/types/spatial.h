#pragma once

#include <ecs/component/component.h>
#include <math/transform.h>

namespace rvr {
class Spatial : public Component {
public:
    Spatial(type::EntityId pId);
    Spatial(type::EntityId pId, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

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

    void UpdateWorld();

private:
    math::Transform local;
    math::Transform world;
};
}