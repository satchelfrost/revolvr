/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <io/action/output/haptic_action.h>

namespace rvr {
class Vibrate : public HapticAction {
public:
    Vibrate(XrActionSet actionSet, HandPathArray handSubactionPath);
};
}