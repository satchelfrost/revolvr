/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

enum class RVRReferenceSpace {
    Head = 0,
    Hud = 1,
    HeadInitial = 2,
    TrackedOrigin = 3,
    StageRight = 4,
    StageLeft = 5,
    StageRightRotated = 6,
    StageLeftRotated = 7
};

inline const char* toString(RVRReferenceSpace space) {
    switch (space) {
    case RVRReferenceSpace::Head:
        return "VRHead";
    case RVRReferenceSpace::Hud:
        return "Hud";
    case RVRReferenceSpace::HeadInitial:
        return "HeadInitial";
    case RVRReferenceSpace::TrackedOrigin:
        return "TrackedOrigin";
    case RVRReferenceSpace::StageRight:
        return "StageRight";
    case RVRReferenceSpace::StageLeft:
        return "StageLeft";
    case RVRReferenceSpace::StageRightRotated:
        return "StateRightRotated";
    case RVRReferenceSpace::StageLeftRotated:
        return "StageLeftRotated";
    default:
        return "Unknown RVRReferenceSpace type";
    }

}