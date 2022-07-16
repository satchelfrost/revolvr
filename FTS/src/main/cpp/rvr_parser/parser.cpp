#include "parser.h"

Parser::Parser(std::string fileName) : scanner_(fileName), fileName_(fileName) {
  tokens_ = scanner_.GetTokens();
}

void Parser::Parse() {
  int iterations = 0;
  while(!tokens_.empty()) {
    Heading heading;
    ParseHeading(heading);
    auto fields = ParseFields(); 
    units_.push_back({heading, fields});
  }
}

Tok Parser::Peek() {
  return tokens_.front().tok;
}

void Parser::Pop() {
  lastToken_ = tokens_.front();
  tokens_.pop();
}

std::string Parser::LineColString() {
  std::string retStr = "error [line: " + std::to_string(tokens_.front().lineNumber);
  retStr += ", column: " + std::to_string(tokens_.front().columNumber) + "]";
  return retStr;
}

void Parser::ParseError(std::string errMsg) {
  throw std::runtime_error(errMsg + " " + LineColString());
}

void Parser::ParseHeading(Heading& heading) {
  // Parse beginning bracket
  if (Peek() != Tok::BrackLeft)
    ParseError("Heading should start with a bracket");
  Pop(); // '['

  // Parse heading type
  ParseHeadingType(heading);

  // Parse heading "key=value" pairs until we hit end bracket
  while(Peek() != Tok::BrackRight) {
    ParseHeadingKeyValuePairs(heading);
  }
  Pop(); // ']'
}

void Parser::ParseHeadingType(Heading& heading) {
  if (Peek() == Tok::Identifier) {
    std::string identifier = scanner_.GetNextIdentifier();
    if (identifier == "node" || identifier == "resource") {
      heading.headingType = identifier;
    }
    else {
      ParseError("Heading type " + identifier + " unrecognized");
    }
    Pop();
  }
  else {
    std::string errMsg = "Expected identifer for heading_type, got ";
    errMsg += Scanner::TokToString(Peek()) + " instead";
    ParseError(errMsg);
  }
}

void Parser::ParseHeadingKeyValuePairs(Heading& heading) {
  // First check for keys
  if (Peek() == Tok::Identifier) {
    std::string identifier = scanner_.GetNextIdentifier();
    // Number values
    if (identifier == "id" || identifier == "parent") {
      Pop();
      if (Peek() == Tok::Equals) {
        Pop();
        if (Peek() == Tok::Number) {
          heading.strKeyNumVal[identifier] = (int)scanner_.GetNextNumber();
          Pop();
        }
      }
    }
    // String keys
    else if (identifier == "name" || identifier == "type") {
      Pop();
      if (Peek() == Tok::Equals) {
        Pop();
        if (Peek() == Tok::Identifier) {
          heading.strKeyStrVal[identifier] = scanner_.GetNextIdentifier();
          Pop();
        }
      }
    }
    else {
      ParseError("Error while parsing header, key " + identifier + " unrecognized");
    }
  }
  else {
    std::string errMsg = "Expected identifer while parsing header keys, got ";
    errMsg += Scanner::TokToString(Peek()) + " instead";
    ParseError(errMsg);
  }
}

std::vector<Parser::Field> Parser::ParseFields() {
  std::vector<Field> fields;
  // While we haven't reached another heading or the end of tokens
  while (Peek() != Tok::BrackLeft && !tokens_.empty()) {
    // All fields should begin with an identifier
    if (Peek() != Tok::Identifier) {
      std::string errMsg = "Expected identifer for field, got ";
      errMsg += Scanner::TokToString(Peek()) + " instead";
    }
    std::string identifier = scanner_.GetNextIdentifier();
    Pop(); // pop identifier

    // Begin parsing various fields
    Field field;
    field.fieldName = identifier;
    if (ParseField1(field)) {
      field.type = FieldType::Field1;
    }
    else if (ParseField2(field)) {
      field.type = FieldType::Field2;
    }
    else if (ParseField3(field)) {
      field.type = FieldType::Field3;
    }
    else if (ParseField4(field)) {
      field.type = FieldType::Field4;
    }
    else if (ParseResource(field)) {
      field.type = FieldType::Resource;
    }
    else if (ParseHand(field)) {
      field.type = FieldType::Hand;
    }
    else {
      std::string errMsg = "Unrecognized field. Token: " + Scanner::TokToString(Peek());
      errMsg += ", field name: " + field.fieldName;
      ParseError(errMsg);
    }
    fields.push_back(field);
  }
  return fields;
}

bool Parser::ParseField1(Field& field) {
  for (auto keyword : {"visible", "custom_type"}) {
    if (field.fieldName == keyword) {
      // Check for left curly brace
      if (Peek() == Tok::CurlLeft) {
        // Here we should get '{' number ''}'
        Pop(); // pop '{'
        if (field.fieldName == "visible")
          ReadBool(field.visible);
        if (field.fieldName == "custom_type")
          ReadBool(field.customType);
      }
      else {
        // Missing left curly brace after field identifier
        ParseError("Expected left curly brace after field1 identifier");
      }
      // Check for right curly brace
      if (Peek() == Tok::CurlRight) {
        Pop();
      }
      else {
        ParseError("Expected right curly brace after number for a field1 identifier");
      }
      return true;
    }
  }
  return false;
}

bool Parser::ParseField2(Field& field) {
  for (auto keyword : {"canvas"}) {
    if (field.fieldName == keyword) {
      // Check for left curly brace
      if (Peek() == Tok::CurlLeft) {
        // Here we should get '{' number ',' number '}'
        Pop(); // pop '{'
        ReadCurlyList(field.vec2.x, true);
        ReadCurlyList(field.vec2.y, false);
      }
      else {
        // Missing left curly brace after field identifier
        ParseError("Expected left curly brace after field2 identifier");
      }
      // Check for right curly brace
      if (Peek() == Tok::CurlRight) {
        Pop();
      }
      else {
        ParseError("Expected right curly brace after number for a field2 identifier");
      }
      return true;
    }
  }
  return false;
}

void Parser::ReadCurlyList(float& f, bool commaExpected) {
  if (Peek() == Tok::Number) {
    f = scanner_.GetNextNumber();
    Pop();
    if (commaExpected && Peek() == Tok::Comma) {
      Pop(); // pop ','
    }
  }
  else {
    // Missing number
    ParseError("Expected a number");
  }
}

void Parser::ReadBool(bool& b) {
  if (Peek() == Tok::Identifier) {
    std::string identifier = scanner_.GetNextIdentifier();
    if (identifier == "true") {
      b = true;
    }
    else if (identifier == "false") {
      b = false;
    }
    else {
      std::string errMsg = "Expected a boolean identifier 'true' or 'false'. Instead received identifier: ";
      errMsg += identifier;
      ParseError(errMsg);
    }
    Pop();
  }
  else {
    std::string errMsg = "Bad token, expected a boolean identifier 'true' or 'false'. Instead received token: ";
    errMsg += Scanner::TokToString(Peek());
    ParseError(errMsg);
  }
}

void Parser::ReadSide(int& side) {
  if (Peek() == Tok::Identifier) {
    std::string identifier = scanner_.GetNextIdentifier();
    if (identifier == "left") {
      side = 0;
    }
    else if (identifier == "right") {
      side = 1;
    }
    else {
      std::string errMsg = "Expected a side identifier 'left' or 'rigth'. Instead received identifier: ";
      errMsg += identifier;
      ParseError(errMsg);
    }
    Pop();
  }
  else {
    std::string errMsg = "Bad token, expected a side identifier 'left' or 'right'. Instead received token: ";
    errMsg += Scanner::TokToString(Peek());
    ParseError(errMsg);
  }
}

bool Parser::ParseField3(Field& field) {
  for (auto keyword : {"scale", "position"}) {
    if (field.fieldName == keyword) {
      // Check for left curly brace
      if (Peek() == Tok::CurlLeft) {
        // Here we should get '{' number ',' number ',' number '}'
        Pop(); // pop '{'
        ReadCurlyList(field.vec3.x, true);
        ReadCurlyList(field.vec3.y, true);
        ReadCurlyList(field.vec3.z, false);
      }
      else {
        // Missing left curly brace after field identifier
        ParseError("Expected left curly brace after field3 identifier");
      }
      // Check for right curly brace
      if (Peek() == Tok::CurlRight) {
        Pop();
      }
      else {
        ParseError("Expected right curly brace after number for a field3 identifier");
      }
      return true;
    }
  }
  return false;
}

bool Parser::ParseField4(Field& field) {
  for (auto keyword : {"orientation"}) {
    if (field.fieldName == keyword) {
      // Check for left curly brace
      if (Peek() == Tok::CurlLeft) {
        // Here we should get '{' number ',' number ',' number ',' number '}'
        Pop(); // pop '{'
        ReadCurlyList(field.quat.x, true);
        ReadCurlyList(field.quat.y, true);
        ReadCurlyList(field.quat.z, true);
        ReadCurlyList(field.quat.w, false);
      }
      else {
        // Missing left curly brace after field identifier
        ParseError("Expected left curly brace after field4 identifier");
      }
      // Check for right curly brace
      if (Peek() == Tok::CurlRight) {
        Pop();
      }
      else {
        ParseError("Expected right curly brace after number for a field4 identifier");
      }
      return true;
    }
  }
  return false;
}

bool Parser::ParseResource(Field& field) {
  for (auto keyword : {"resource"}) {
    if (field.fieldName == keyword) {
      // Check for left curly brace
      if (Peek() == Tok::CurlLeft) {
        // Here we should get '{' number '}'
        Pop(); // pop '{'
        float resource;
        ReadCurlyList(resource, false);
        field.resource = (int)resource;
      }
      else {
        // Missing left curly brace after field identifier
        ParseError("Expected left curly brace after resource identifier");
      }
      // Check for right curly brace
      if (Peek() == Tok::CurlRight) {
        Pop();
      }
      else {
        ParseError("Expected right curly brace after number for a resource identifier");
      }
      return true;
    }
  }
  return false;
}

bool Parser::ParseHand(Field& field) {
  for (auto keyword : {"hand"}) {
    if (field.fieldName == keyword) {
      // Check for left curly brace
      if (Peek() == Tok::CurlLeft) {
        // Here we should get '{' number '}'
        Pop(); // pop '{'
        ReadSide(field.hand);
      }
      else {
        // Missing left curly brace after field identifier
        ParseError("Expected left curly brace after resource identifier");
      }
      // Check for right curly brace
      if (Peek() == Tok::CurlRight) {
        Pop();
      }
      else {
        ParseError("Expected right curly brace after number for a resource identifier");
      }
      return true;

    }
  }
  return false;
}

void Parser::PrintTokens() {
  auto tokens = Scanner(fileName_).GetTokens();
  while (!tokens_.empty()) {
    auto token = Peek();
    if (token == Tok::Identifier)
      std::cout << Scanner::TokToString(token) << ", " << scanner_.GetNextIdentifier() << "\n";
    else if (token == Tok::Number)
      std::cout << Scanner::TokToString(token) << ", " << scanner_.GetNextNumber() << "\n";
    else
      std::cout << Scanner::TokToString(token) << "\n";
    Pop();
  }
}

std::vector<Parser::Unit>& Parser::GetUnits() {
  return units_;
}