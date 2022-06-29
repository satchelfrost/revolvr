#pragma once

enum class RVRReferenceSpace {
    RVRHead = 0,
    RVRHud = 1,
    RVRHeadInitial = 2,
    RVRTrackedOrigin = 3,
    RVRStageRight = 4,
    RVRStageLeft = 5,
    RVRStageRightRotated = 6,
    RVRStageLeftRotated = 7
};

inline const char* toString(RVRReferenceSpace space) {
    switch (space) {
    case RVRReferenceSpace::RVRHead:
        return "RVRHead";
    case RVRReferenceSpace::RVRHud:
        return "RVRHud";
    case RVRReferenceSpace::RVRHeadInitial:
        return "RVRHeadInitial";
    case RVRReferenceSpace::RVRTrackedOrigin:
        return "RVRTrackedOrigin";
    case RVRReferenceSpace::RVRStageRight:
        return "RVRStageRight";
    case RVRReferenceSpace::RVRStageLeft:
        return "RVRStageLeft";
    case RVRReferenceSpace::RVRStageRightRotated:
        return "RVRStateRightRotated";
    case RVRReferenceSpace::RVRStageLeftRotated:
        return "RVRStageLeftRotated";
    default:
        return "Unknown RVRReferenceSpace type";
    }

}