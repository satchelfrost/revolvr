#pragma once

#include "rvr_math.h"
#include "rvr_reference_space.h"


inline XrReferenceSpaceCreateInfo GetXrReferenceSpaceCreateInfo(RVRReferenceSpace referenceSpace) {
    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::Identity();
    XrVector3f stageLeft{-2.f, 0.f, -2.f};
    XrVector3f stageRight{2.f, 0.f, -2.f};
    XrVector3f forward2{0.f, 0.f, -2.f};
    float piOver3 = 3.14 / 3.f;

    switch (referenceSpace) {
    case RVRReferenceSpace::RVRHead:
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        break;
    case RVRReferenceSpace::RVRHud:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::Translation(forward2);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        break;
    case RVRReferenceSpace::RVRHeadInitial:
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        break;
    case RVRReferenceSpace::RVRTrackedOrigin:
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        break;
    case RVRReferenceSpace::RVRStageLeft:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::RotateCCWAboutYAxis(0.f, stageLeft);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        break;
    case RVRReferenceSpace::RVRStageRight:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::RotateCCWAboutYAxis(0.f, stageRight);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        break;
    case RVRReferenceSpace::RVRStageLeftRotated:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::RotateCCWAboutYAxis(piOver3, stageLeft);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
            break;
    case RVRReferenceSpace::RVRStageRightRotated:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::RotateCCWAboutYAxis(-piOver3, stageRight);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        break;
    default:
        ALOGE("Unknown reference space type '%d', see RVRReferenceSpace enum class", referenceSpace);
        exit(1);
    }
    return referenceSpaceCreateInfo;
}
