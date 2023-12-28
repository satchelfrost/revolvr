/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/system/lighting_system.h>
#include <global_context.h>

namespace rvr::sys::lighting {
void AppendLightSources(std::vector<PointLight*>& pointLights) {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::PointLight);
    for (auto& [eid, component] : components) {
        if (pointLights.size() < MAX_LIGHTS)
            pointLights.push_back(reinterpret_cast<PointLight*>(component));
        else
            PrintWarning("Maximum light count exceeded");
    }
}
}
