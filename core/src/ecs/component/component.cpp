/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "ecs/component/component.h"

namespace rvr {
Component::Component(ComponentType pType, type::EntityId pId) : type(pType), id(pId) {}
}