/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include "pch.h"
#include <rendering/utilities/geometry.h>

namespace rvr::system::point_cloud {
std::vector<Geometry::Vertex> getVertexDataFromPly(const std::string& fileName);
}
