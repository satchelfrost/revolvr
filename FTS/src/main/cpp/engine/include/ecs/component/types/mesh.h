#pragma once

#include <ecs/component/component.h>

namespace rvr {
class Mesh : public Component {
public:
    Mesh(const Mesh& other) = delete;
    Mesh(const Mesh& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    Mesh(type::EntityId pId);
    void SetVisibilityRecursive(bool visibility);
    bool visible;
private:
    //Resource resource_
};
}
