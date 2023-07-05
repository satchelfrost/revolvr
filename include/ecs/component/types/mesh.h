/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <ecs/component/component.h>

namespace rvr {
class Mesh : public Component {
public:
    Mesh(type::EntityId pId, bool visible=true);
    Mesh(const Mesh& other) = delete;
    Mesh(const Mesh& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;
    bool IsVisible() const;

    void SetVisibilityRecursive(bool visibility);
private:
    bool visible_;
    //Resource resource_
};
}
