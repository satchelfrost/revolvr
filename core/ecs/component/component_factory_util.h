/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <ecs/entity/entity.h>
#include <scene/parser.h>
#include "color_list.h"
#include "glm/vec3.hpp"

namespace rvr::componentFactory {
// Returns true if string with nameQualifier was found and sets the out string with found value
bool GetStringField(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                    const std::string& nameQualifier, std::string& outStr);

// Returns true if boolean with nameQualifier was found and sets the out boolean with found value
bool GetBoolField(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                  const std::string& nameQualifier, bool& outBool);

// Returns true if 3 floats with nameQualifier was found and sets the out floats with found values
bool GetFloat3Field(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                    const std::string& nameQualifier, float& outFloat1, float& outFloat2, float& outFloat3);

// Returns true if 4 floats with nameQualifier was found and sets the out floats with found values
bool GetFloat4Field(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                    const std::string& nameQualifier, float& outFloat1, float& outFloat2, float& outFloat3,
                    float& outFloat4);

// Returns true if 1 float with nameQualifier was found and sets the out float with found value
bool GetFloatField(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                   const std::string& nameQualifier, float& outFloat);

glm::vec3 GetNormalizedRGBFromColorName(const std::string& colorName);
}