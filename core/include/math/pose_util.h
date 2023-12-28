/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once
#include "pch.h"
#include <glm/glm.hpp>

namespace rvr::math::pose {
    // Returns the identity XrPosef defined as position {0,0,0} and orientation {0,0,0,1}
    XrPosef Identity();

    // Creates a XrPosef whose position is @translation
    XrPosef Translation(const XrVector3f& translation);

    XrPosef RotateCCWAboutYAxis(float radians, XrVector3f translation);
}
