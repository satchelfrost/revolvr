#include "parser.h"

Parser::Parser(const std::string& fileName) : scanner_(fileName), fileName_(fileName) {
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
  for (auto keyword : {"node", "resource"})
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
  for (auto keyword : {"name", "type"}) {
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
  // While we haven't reached another heading or the end of tokens
  while (Peek() != Token::BrackLeft && !tokens_.empty()) {
    // All fields begin with identifier
    CheckPeek("Parsing fields", Token::Identifier);

    // Begin parsing various fields
    Field field;
    field.fieldName = Pop().GetIdentifier();

    if (ParseField1(field))
      field.type = Field1;
    else if (ParseField2(field))
      field.type = Field2;
    else if (ParseField3(field))
      field.type = Field3;
    else if (ParseField4(field))
      field.type = Field4;
    else if (ParseResourceId(field))
      field.type = Resource;
    else if (ParseHand(field))
      field.type = Hand;
    else
      ParseErrorPrevToken("Unrecognized field \"" + field.fieldName + "\"");
    fields.push_back(field);
  }
  return fields;
}

bool Parser::ParseField1(Field& field) {
  // keyword '{' bool ''}'
  for (auto keyword : {"visible", "custom_type"}) {
    if (field.fieldName == keyword) {
      CheckPop("Field1", Token::CurlLeft);
      ReadBool(field.boolean);
      CheckPop("Field1", Token::CurlRight);
      return true;
    }
  }
  return false;
}

bool Parser::ParseField2(Field& field) {
  // keyword '{' number ',' number '}'
  for (auto keyword : {"canvas"}) {
    if (field.fieldName == keyword) {
      CheckPop("Field2", Token::CurlLeft);
      ReadCurlyList(field.vec2.x, true);
      ReadCurlyList(field.vec2.y, false);
      CheckPop("Field2", Token::CurlRight);
      return true;
    }
  }
  return false;
}

bool Parser::ParseField3(Field& field) {
  // keyword '{' number ',' number ',' number '}'
  for (auto keyword : {"scale", "position"}) {
    if (field.fieldName == keyword) {
      CheckPop("Field3", Token::CurlLeft);
      ReadCurlyList(field.vec3.x, true);
      ReadCurlyList(field.vec3.y, true);
      ReadCurlyList(field.vec3.z, false);
      CheckPop("Field3", Token::CurlRight);
      return true;
    }
  }
  return false;
}

bool Parser::ParseField4(Field& field) {
  // keyword '{' number ',' number ',' number ',' number '}'
  for (auto keyword : {"orientation"}) {
    if (field.fieldName == keyword) {
      CheckPop("Field4", Token::CurlLeft);
      ReadCurlyList(field.quat.x, true);
      ReadCurlyList(field.quat.y, true);
      ReadCurlyList(field.quat.z, true);
      ReadCurlyList(field.quat.w, false);
      CheckPop("Field4", Token::CurlRight);
      return true;
    }
  }
  return false;
}

bool Parser::ParseResourceId(Field& field) {
  // keyword '{' number '}'
  for (auto keyword : {"resource_id"}) {
    if (field.fieldName == keyword) {
      CheckPop("Field resource.", Token::CurlLeft);
      float resource_id;
      ReadCurlyList(resource_id, false);
      field.resource_id = (int)resource_id;
      CheckPop("Field resource_id.", Token::CurlRight);
      return true;
    }
  }
  return false;
}

bool Parser::ParseHand(Field& field) {
  // keyword '{' side '}'
  for (auto keyword : {"hand"}) {
    if (field.fieldName == keyword) {
      CheckPop("Field hand", Token::CurlLeft);
      ReadSide(field.hand);
      CheckPop("Field hand", Token::CurlRight);
      return true;
    }
  }
  return false;
}

void Parser::ReadCurlyList(float& number, bool commaExpected) {
  CheckPeek("Expected a number", Token::Number);
  number = (float)Pop().GetNumber();
  if (commaExpected)
    CheckPop("Read Curly List", Token::Comma);
}

void Parser::ReadBool(bool& b) {
  CheckPeek("Read bool", Token::Identifier);
  std::string identifier = Pop().GetIdentifier();
  if (identifier == "true")
    b = true;
  else if (identifier == "false")
    b = false;
  else
    ParseErrorPrevToken("Boolean identifier \"" + identifier + "\" unrecognized. Use true or false.");
}

void Parser::ReadSide(int& side) {
  CheckPeek("Read side", Token::Identifier);
  std::string identifier = Pop().GetIdentifier();
  if (identifier == "left")
    side = 0;
  else if (identifier == "right")
    side = 1;
  else
    ParseErrorPrevToken("Side identifier \"" + identifier + "\" unrecognized. Use left of right.");
}

void Parser::PrintTokens() {
  Scanner scanner(fileName_);
  auto tokens = scanner.GetTokens();
  while (!tokens.empty()) {
    auto token = tokens.front();
    tokens.pop();
    Log::Write(Log::Level::Info, Fmt("Token name: %s", Token::StringFromTokEnum(token.tok).c_str()));
    Log::Write(Log::Level::Info, Fmt("Token value: %s", token.ToString().c_str()));
    Log::Write(Log::Level::Info, token.LineColString());
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