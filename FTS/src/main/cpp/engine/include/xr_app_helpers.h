#pragma once

#include "common.h"
#include "math/rvr_math.h"
#include "rvr_reference_space.h"

inline XrReferenceSpaceCreateInfo GetXrReferenceSpaceCreateInfo(RVRReferenceSpace referenceSpace) {
    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::Identity();
    XrVector3f stageLeft{-2.f, 0.f, -2.f};
    XrVector3f stageRight{2.f, 0.f, -2.f};
    XrVector3f forward2{0.f, 0.f, -2.f};
    float piOver3 = 3.14 / 3.f;

    switch (referenceSpace) {
    case RVRReferenceSpace::Head:
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        break;
    case RVRReferenceSpace::Hud:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::Translation(forward2);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        break;
    case RVRReferenceSpace::HeadInitial:
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        break;
    case RVRReferenceSpace::TrackedOrigin:
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        break;
    case RVRReferenceSpace::StageLeft:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::RotateCCWAboutYAxis(0.f, stageLeft);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        break;
    case RVRReferenceSpace::StageRight:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::RotateCCWAboutYAxis(0.f, stageRight);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        break;
    case RVRReferenceSpace::StageLeftRotated:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::RotateCCWAboutYAxis(piOver3, stageLeft);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
            break;
    case RVRReferenceSpace::StageRightRotated:
        referenceSpaceCreateInfo.poseInReferenceSpace = RVRMath::Pose::RotateCCWAboutYAxis(-piOver3, stageRight);
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        break;
    default:
        THROW(Fmt("Unknown reference space type '%d', see RVRReferenceSpace enum class", referenceSpace));
    }
    return referenceSpaceCreateInfo;
}

struct TrackedSpaceLocations {
    XrSpaceLocation leftHand{XR_TYPE_SPACE_LOCATION};
    XrSpaceLocation rightHand{XR_TYPE_SPACE_LOCATION};
    XrSpaceLocation vrOrigin{XR_TYPE_SPACE_LOCATION};

    enum TrackedSpaceLocation {
        LeftHand,
        RightHand,
        VrOrigin
    };

    static bool ValidityCheck(XrResult res, XrSpaceLocation location) {
        CHECK_XRRESULT(res, "xrLocateSpace");
        return XR_UNQUALIFIED_SUCCESS(res) &&
               (location.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) &&
               (location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT);
    }

    std::vector<TrackedSpaceLocation> locations = {LeftHand, RightHand, VrOrigin};
};
