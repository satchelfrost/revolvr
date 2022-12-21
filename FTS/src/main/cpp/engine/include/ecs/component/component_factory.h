#include <ecs/entity/entity.h>
#include <rvr_parser/parser.h>

#define CREATE_COMPONENT(TYPE, NUM) void Create ## TYPE(Entity* entity, const rvr::Parser::Field& field);

// We are merely initializing the components, not creating them.
// Component creation is done implicity by creating an entity.
namespace rvr::componentFactory {
COMPONENT_LIST(CREATE_COMPONENT)
}
