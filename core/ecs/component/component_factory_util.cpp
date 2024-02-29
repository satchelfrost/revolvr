/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <ecs/component/component_factory_util.h>

namespace rvr::componentFactory {
// Returns true if string with nameQualifier was found and sets the out string with found value
bool GetStringField(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                           const std::string& nameQualifier, std::string& outStr)  {
    auto strField = fields.find(nameQualifier);
    if (strField != fields.end()) {
        try {
            outStr = strField->second.strValues.at(0);
            return true;
        }
        catch (std::out_of_range& e) {
            THROW(Fmt("Out of bounds %s was expecting 1 string for entity %s",
                      nameQualifier.c_str(), entity->GetName().c_str()));
        }
    }
    return false;
}

// Returns true if boolean with nameQualifier was found and sets the out boolean with found value
bool GetBoolField(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                         const std::string& nameQualifier, bool& outBool) {
    std::string boolStr;
    if (GetStringField(entity, fields, nameQualifier, boolStr)) {
        outBool = (boolStr == "true");
        return true;
    }
    else {
        return false;
    }
}

// Returns true if 3 floats with nameQualifier was found and sets the out floats with found values
bool GetFloat3Field(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                           const std::string& nameQualifier, float& outFloat1, float& outFloat2, float& outFloat3)  {
    auto float3Field = fields.find(nameQualifier);
    if (float3Field != fields.end()) {
        try {
            outFloat1 = float3Field->second.floatValues.at(0);
            outFloat2 = float3Field->second.floatValues.at(1);
            outFloat3 = float3Field->second.floatValues.at(2);
            return true;
        }
        catch (std::out_of_range& e) {
            THROW(Fmt("Out of bounds %s was expecting 3 floats entity %s",
                      nameQualifier.c_str(), entity->GetName().c_str()));
        }
    }
    return false;
}

bool GetFloat4Field(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                    const std::string& nameQualifier, float& outFloat1, float& outFloat2, float& outFloat3,
                    float& outFloat4)  {
    auto float3Field = fields.find(nameQualifier);
    if (float3Field != fields.end()) {
        try {
            outFloat1 = float3Field->second.floatValues.at(0);
            outFloat2 = float3Field->second.floatValues.at(1);
            outFloat3 = float3Field->second.floatValues.at(2);
            outFloat4 = float3Field->second.floatValues.at(3);
            return true;
        }
        catch (std::out_of_range& e) {
            THROW(Fmt("Out of bounds %s was expecting 4 floats entity %s",
                      nameQualifier.c_str(), entity->GetName().c_str()));
        }
    }
    return false;
}

// Returns true if 1 float with nameQualifier was found and sets the out float with found value
bool GetFloatField(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                          const std::string& nameQualifier, float& outFloat)  {
    auto floatField = fields.find(nameQualifier);
    if (floatField != fields.end()) {
        try {
            outFloat = floatField->second.floatValues.at(0);
            return true;
        }
        catch (std::out_of_range& e) {
            THROW(Fmt("Out of bounds %s was expecting 1 floats entity %s",
                      nameQualifier.c_str(), entity->GetName().c_str()));
        }
    }
    return false;
}

glm::vec3 GetNormalizedRGBFromColorName(const std::string& colorName) {
#define TO_RGB_FROM_NAME(NAME, R, G, B) if (colorName == #NAME) return {R / 255.0f, G / 255.0f, B / 255.0f};
    COLOR_LIST(TO_RGB_FROM_NAME)
#undef TO_RGB_FROM_NAME

    PrintWarning("Color " + colorName + " was not in color_list.h. Please update.");
    return {1.0f, 1.0f, 1.0f};
}
}