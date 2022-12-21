#include <ecs/entity/entity.h>
#include <rvr_parser/parser.h>

#define CREATE_COMPONENT(TYPE, NUM) void Create ## TYPE(Entity* entity, const std::map<std::string, rvr::Parser::Field>& fields);

namespace rvr::componentFactory {
COMPONENT_LIST(CREATE_COMPONENT)
}
