/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <io/action/output/oculus_output_actions.h>

namespace rvr {
rvr::Vibrate::Vibrate(XrActionSet actionSet, HandPathArray handSubactionPath) :
HapticAction(actionSet, handSubactionPath, "/output/haptic", ActionType::Vibrate){}
}
