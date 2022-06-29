#pragma once

enum class RVRType {
    Object = 0,
    Spatial = 1,
    Mesh = 2,
    Origin = 3,
    Hand = 4
};

inline const char* toString(RVRType rvrType) {
    switch (rvrType) {
    case RVRType::Object:
        return "Object";
    case RVRType::Spatial:
        return "Spatial";
    case RVRType::Mesh:
        return "Mesh";
    case RVRType::Origin:
        return "Origin";
    case RVRType::Hand:
        return "Hand";
    default:
        return "RVRType unrecognized";
    }
}