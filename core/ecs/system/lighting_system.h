/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include "ecs/entity/entity.h"
#include "ecs/component/types/point_light.h"

namespace rvr::sys::lighting {
void AppendLightSources(std::vector<PointLight*>& pointLights);
}
