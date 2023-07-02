/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include "ecs/entity/entity.h"
#include "ecs/component/types/ritual.h"

namespace rvr::system::ritual{
void Update(float dt);
void Begin();
}

