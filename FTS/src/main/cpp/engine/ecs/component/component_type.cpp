#include <ecs/ecs_info.h>
#include <common.h>

namespace rvr {
const char* toString(ComponentType cType) {
    int index = (int)cType;
    if (index < 0 || index > constants::IMPLEMENTED_COMPONENTS)
        return "ComponentType unrecognized";
    return componentInfo[index].str;
}

ComponentType toComponentTypeEnum(const std::string& str) {
    for (int i = 0; i < constants::IMPLEMENTED_COMPONENTS; i++) {
        std::string enumStr = toString((ComponentType)i);
        if (enumStr == str)
            return (ComponentType)i;
    }
    THROW(Fmt("No component type found for %s", str.c_str()));
}
}
