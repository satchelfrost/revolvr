/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/system/render_system.h>
#include <ecs/component/types/mesh.h>
#include <global_context.h>

namespace rvr::system::render{
std::vector<Spatial *> GetRenderSpatials() {
    std::vector<Spatial*> spatials;
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Mesh);
    spatials.reserve(components.size());
    for (auto [eid, component] : components) {
        auto mesh = dynamic_cast<Mesh*>(component);
        if (mesh->IsVisible())
            spatials.push_back(GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(eid));
    }
    return spatials;
}
}