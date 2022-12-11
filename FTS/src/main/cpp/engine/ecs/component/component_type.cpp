#include "ecs/component/component_type.h"
#include "common.h"

#define COMPONENT_TYPE_CASE_STR(ENUM, NUM) case ComponentType::ENUM: return #ENUM;

namespace rvr {
const char *toString(ComponentType cType) {
    switch (cType) {
        COMPONENT_LIST(COMPONENT_TYPE_CASE_STR)
        default:
            return "ComponentType unrecognized";
    }
}

ComponentType toComponentTypeEnum(const std::string& str) {
    for (int i = 0; i < constants::IMPLEMENTED_COMPONENTS; i++) {
        std::string enumStr = toString((ComponentType)i);
        if (enumStr == str)
            return (ComponentType)i;
    }
    THROW(Fmt("No tracked space found for %s", str.c_str()));
}
}