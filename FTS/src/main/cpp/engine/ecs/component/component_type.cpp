#include "ecs/component/component_type.h"

#define COMPONENT_TYPE_CASE_STR(ENUM, NUM) case ComponentType::ENUM: return #ENUM;

namespace rvr {
const char *toString(ComponentType cType) {
    switch (cType) {
        COMPONENT_LIST(COMPONENT_TYPE_CASE_STR)
        default:
            return "ComponentType unrecognized";
    }
}
}