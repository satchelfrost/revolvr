#pragma once

enum class RVRType {
    RVRObject = 0,
    RVRSpatial = 1,
    RVRMesh = 2
};

inline const char* toString(RVRType rvrType) {
    switch (rvrType) {
    case RVRType::RVRObject:
        return "RVRObject";
    case RVRType::RVRSpatial:
        return "RVRSpatial";
    case RVRType::RVRMesh:
        return "RVRMesh";
    default:
        return "RVRType unrecognized";
    }
}