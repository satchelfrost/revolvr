/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include "ecs/entity/entity.h"
#include "ecs/component/types/spatial.h"

namespace rvr::system::render{
void PopulateRenderTransformBuffer(std::vector<math::Transform>& buffer);
void DrawGrid(std::vector<math::Transform>& buffer);
}
