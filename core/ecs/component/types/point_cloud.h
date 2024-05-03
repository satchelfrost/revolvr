/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <ecs/component/component.h>

namespace rvr {
class PointCloud : public Component {
public:
    enum FileType {
        Ply,
        Las,
        Laz,
        E57,
        Vtx
    };
    PointCloud(type::EntityId pId, std::string  resourceName, FileType fileType);
    PointCloud(const PointCloud& other) = delete;
    PointCloud(const PointCloud& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    std::string FullResourceName();
    bool IsVisible() const;
    bool HasResource() const;
    void SetVisibilityRecursive(bool visibility);
    std::string GetFileExtension();
    FileType GetFileType();

private:
    bool visible_;
    std::string resourceName_;
    FileType fileType_;
};
}
