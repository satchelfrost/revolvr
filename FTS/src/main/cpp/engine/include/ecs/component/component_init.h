#include <ecs/entity/entity.h>
#include <rvr_parser/parser.h>

#define INIT_COMPONENT(TYPE, NUM) void Init ## TYPE(Entity* entity, rvr::Parser::Field field);

// We are merely initializing the components, not creating them.
// Component creation is done implicity by creating an entity.
namespace rvr::componentInit {
COMPONENT_LIST(INIT_COMPONENT)
}
