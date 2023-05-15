#include <ecs/entity/entity.h>
#include <rvr_parser/parser.h>

namespace rvr::componentFactory {
#define CREATE_COMPONENT_FUNC(TYPE) void Create ## TYPE(Entity* entity, const std::map<std::string, rvr::Parser::Field>& fields);
COMPONENT_LIST(CREATE_COMPONENT_FUNC)
#undef CREATE_COMPONENT_FUNC
}
