/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#include <ecs/entity/entity.h>
#include <scene/parser.h>

namespace rvr::componentFactory {
#define CREATE_COMPONENT_FUNC(TYPE) void Create ## TYPE(Entity* entity, const std::map<std::string, rvr::Parser::Field>& fields);
COMPONENT_LIST(CREATE_COMPONENT_FUNC)
#undef CREATE_COMPONENT_FUNC
}
