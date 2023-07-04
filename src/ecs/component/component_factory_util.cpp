/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

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
}