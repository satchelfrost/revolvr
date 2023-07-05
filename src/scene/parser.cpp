/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <scene/parser.h>

namespace rvr {
Parser::Parser(const std::string& fileName) : scanner_(fileName) {
  tokens_ = scanner_.GetTokens();
  prevToken_ = tokens_.front();
}

std::vector<Parser::Unit> Parser::Parse() {
  while(!tokens_.empty()) {
    Heading heading;
    ParseHeading(heading);
    auto fields = ParseFields(); 
    units_.push_back({heading, fields});
  }
  return units_;
}

Token::Tok Parser::Peek() {
  return tokens_.front().tok;
}

Token Parser::Pop() {
  prevToken_ = tokens_.front();
  tokens_.pop();
  return prevToken_;
}

void Parser::ParseHeading(Heading& heading) {
  CheckPop("Heading", Token::BrackLeft);
  ParseHeadingType(heading);
  while(Peek() != Token::BrackRight)
    ParseHeadingKeyValuePairs(heading);
  Pop(); // ']'
}

void Parser::ParseHeadingType(Heading& heading) {
  CheckPeek("Heading Type", Token::Identifier);
  heading.headingType = Pop().GetIdentifier();
  for (auto keyword : {"entity", "resource"})
    if (keyword == heading.headingType)
      return;
  ParseErrorPrevToken("Heading type \"" + heading.headingType + "\" unrecognized");
}

void Parser::ParseHeadingKeyValuePairs(Heading& heading) {
  CheckPeek("Heading", Token::Identifier);
  std::string key = Pop().GetIdentifier();
  // Check for number as value
  for (auto keyword : {"id", "parent"}) {
    if (key == keyword) {
      CheckPop("Heading", Token::Equals);
      CheckPeek("Heading", Token::Number);
      heading.strKeyNumVal[key] = (int)Pop().GetNumber();
      return;
    }
  }
  // Check for string as value
  for (auto keyword : {"name"}) {
    if (key == keyword) {
      CheckPop("Heading", Token::Equals);
      CheckPeek("Heading", Token::Identifier);
      heading.strKeyStrVal[key] = Pop().GetIdentifier();
      return;
    }
  }
  ParseErrorPrevToken("Header key \"" + key + "\" unrecognized");
}

std::map<ComponentType, std::map<std::string, Parser::Field>> Parser::ParseFields() {
  std::map<ComponentType, std::map<std::string, Field>> fields;
  while (Peek() != Token::BrackLeft && !tokens_.empty()) {
    auto field = ParseField();
    if (field.fullyQualifiedName.empty())
      continue;
    fields[field.cType].emplace(field.fullyQualifiedName, field);
  }
  return fields;
}

Parser::Field Parser::ParseField() {
  // Check for a comment
  if (Peek() == Token::Comment) {
    Pop();
    return {}; // simply return an empty field
  }

  // All fields begin with identifier which is the component type
  CheckPeek("Parsing fields", Token::Identifier);

  // Create a field
  Field field{};
  std::string componentTypeStr = Pop().GetIdentifier();
  field.cType = toComponentTypeEnum(componentTypeStr);
  field.fullyQualifiedName = componentTypeStr;
  ParseAccess(field); // builds the remaining fully qualified name

  // Parse left curly brace
  CheckPop("Field Curly Left", Token::CurlLeft);

  // Field can either have a list of strings, floats, or empty e.g. {}, {true}, {1, 1, 1}, etc.
  switch (Peek()) {
      case Token::Identifier:
        ParseAccessStrValues(field);
        break;
      case Token::Number:
        ParseAccessFloatValues(field);
        break;
    default:
      CheckPop("Field Curly Right", Token::CurlRight);
  }

  return field;
}

void Parser::ParseAccessStrValues(Field& field) {
  CheckPeek("Expected an access string value", Token::Identifier);
  field.strValues.push_back(Pop().GetIdentifier());
  while (Peek() == Token::Comma) {
    CheckPop("Read String Curly List", Token::Comma);
    CheckPeek("Expected an access string value", Token::Identifier);
    field.strValues.push_back(Pop().GetIdentifier());
  }
  CheckPop("Field Curly Right", Token::CurlRight);
}

void Parser::ParseAccessFloatValues(Field& field) {
  CheckPeek("Expected a number", Token::Number);
  field.floatValues.push_back((float)Pop().GetNumber());
  while (Peek() == Token::Comma) {
    CheckPop("Read Number Curly List", Token::Comma);
    CheckPeek("Expected a number", Token::Number);
    field.floatValues.push_back((float)Pop().GetNumber());
  }
  CheckPop("Field Curly Right", Token::CurlRight);
}

void Parser::ParseAccess(Field& field) {
  if (Peek() == Token::Dot) {
        Pop();
        CheckPeek("Parsing Access", Token::Identifier);
        field.fullyQualifiedName += "." + Pop().GetIdentifier();
        if (Peek() != Token::Dot)
          return;
        ParseAccess(field);
  }
}

void Parser::ParseErrorPrevToken(const std::string& errMsg) {
  THROW(Fmt("%s %s", errMsg.c_str(), prevToken_.LineColString().c_str()));
}

void Parser::TokenError(std::string errMsg, Token::Tok expected) {
  errMsg += " parse error. Expected \"" + Token::StringFromTokEnum(expected);
  errMsg += "\", received \"" + Token::StringFromTokEnum(Peek()) + "\"";
  ParseErrorPrevToken(errMsg);
}

void Parser::CheckPeek(const char* context, Token::Tok expected) {
  if (Peek() != expected)
    TokenError(context, expected);
}

void Parser::CheckPop(const char* context, Token::Tok expected) {
  if (Peek() != expected)
    TokenError(context, expected);
  Pop();
}
}