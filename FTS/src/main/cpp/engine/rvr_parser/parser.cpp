#include "rvr_parser/parser.h"

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

std::vector<Parser::Field> Parser::ParseFields() {
  std::vector<Field> fields;
  while (Peek() != Token::BrackLeft && !tokens_.empty())
      fields.push_back(ParseField());
  return fields;
}

Parser::Field Parser::ParseField() {
  // All fields begin with identifier which is the component type
  CheckPeek("Parsing fields", Token::Identifier);

  Field field{};
  field.cType = toComponentTypeEnum(Pop().GetIdentifier());
  field.access = nullptr;
  ParseAccess(field.access);

  // Parse left curly brace
  CheckPop("Field Curly Left", Token::CurlLeft);

  if (field.access == nullptr) {
    // In this case we are not assigning a value e.g. "Mesh {}"
    CheckPop("Field Curly Right", Token::CurlRight);
    return field;
  }

  // Get last access that will contain value
  Access* accessWithValue = field.access;
  while (accessWithValue->access != nullptr)
    accessWithValue = accessWithValue->access;

  // Access can either have a list of strings or floats e.g. {SomeClass}, {true}, {1, 1, 1}, etc.
  if (Peek() == Token::Identifier) {
      ParseAccessStrValues(accessWithValue);
  }
  else if (Peek() == Token::Number) {
      ParseAccessFloatValues(accessWithValue);
  }
  else {
      ParseErrorPrevToken("Expected either a string or a value");
  }

  // value should end with a right curly
  CheckPop("Field Curly Right", Token::CurlRight);
  return field;
}

void Parser::ParseAccessStrValues(Access* access) {
  CheckPeek("Expected an access string value", Token::Identifier);
  access->strValues.push_back(Pop().GetIdentifier());
  while (Peek() != Token::Comma) {
    CheckPop("Read String Curly List", Token::Comma);
    CheckPeek("Expected an access string value", Token::Identifier);
    access->strValues.push_back(Pop().GetIdentifier());
  }
}

void Parser::ParseAccessFloatValues(Access* access) {
  CheckPeek("Expected a number", Token::Number);
  access->floatValues.push_back((float)Pop().GetNumber());
  while (Peek() != Token::Comma) {
    CheckPop("Read Number Curly List", Token::Comma);
    CheckPeek("Expected a number", Token::Number);
    access->floatValues.push_back((float)Pop().GetNumber());
  }
}

void Parser::ParseAccess(Access* access) {
    // avoid overwriting pre-allocated memory
    if (access != nullptr)
      return;

    if (Peek() == Token::Dot) {
        Pop();
        CheckPeek("Parsing Access", Token::Identifier);
        access = new Access();
        access->accessName = Pop().GetIdentifier();
        access->access = nullptr;
        ParseAccess(access->access);
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