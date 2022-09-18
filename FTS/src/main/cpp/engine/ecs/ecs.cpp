#include "ecs/ecs.h"

namespace rvr {
const char* toString(ComponentType cType) {
    switch (cType) {
        case ComponentType::Spatial:
            return "Spatial";
        case ComponentType::Mesh:
            return "Mesh";
        case ComponentType::Origin:
            return "Origin";
        case ComponentType::Hand:
            return "Hand";
        default:
            return "ComponentType unrecognized";
    }
}
}