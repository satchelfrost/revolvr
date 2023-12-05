/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <ecs/component/component.h>

namespace rvr {
class Mesh : public Component {

public:
    enum Primitive {
        Quad,
        Cube
    };

    Mesh(type::EntityId pId, bool visible=true);
    Mesh(const Mesh& other) = delete;
    Mesh(const Mesh& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;
    bool IsVisible() const;
    bool HasResource() const;
    std::string ResourceName();
    void SetName(std::string name);
    void SetPrimitiveType(Primitive primitive);
    Primitive GetPrimitiveType();

    void SetVisibilityRecursive(bool visibility);
private:
    bool visible_;
    std::string resourceName_;
    Primitive primitive_;
};
}
