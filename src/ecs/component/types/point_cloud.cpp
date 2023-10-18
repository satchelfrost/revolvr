/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/component/types/point_cloud.h>
#include <global_context.h>

#include <utility>

namespace rvr {
PointCloud::PointCloud(type::EntityId pId, std::string  resourceName, FileType fileType) :
Component(ComponentType::PointCloud, pId), resourceName_(std::move(resourceName)), visible_(true),
fileType_(fileType) {}

PointCloud::PointCloud(const PointCloud& other, type::EntityId newEntityId) :
        Component(ComponentType::PointCloud, newEntityId), resourceName_(other.resourceName_),
        visible_(other.visible_), fileType_(other.fileType_) {}

Component *PointCloud::Clone(type::EntityId newEntityId) {
    return new PointCloud(*this, newEntityId);
}

void PointCloud::SetVisibilityRecursive(bool visibility) {
    auto entity = GlobalContext::Inst()->GetECS()->GetEntity(id);
    for (auto child : entity->GetChildren()) {
        auto mesh = GlobalContext::Inst()->GetECS()->GetComponent<PointCloud>(child->id);
        CHECK_MSG(mesh, Fmt("Entity with id %d has no mesh", child->id));
        mesh->SetVisibilityRecursive(visibility);
    }
    visible_ = visibility;
}

bool PointCloud::IsVisible() const {
    return visible_;
}

bool PointCloud::HasResource() const {
    return !resourceName_.empty();
}

std::string PointCloud::FullResourceName() {
    return resourceName_ + "." + GetFileExtension();
}

std::string PointCloud::GetFileExtension() {
    switch (fileType_) {
    case Ply: return "ply";
    case Las: return "las";
    case Laz: return "laz";
    case E57: return "e57";
    default:
        PrintError("Point cloud file type unrecognized " + std::to_string(fileType_)) ;
        return "";
    }
}
}
