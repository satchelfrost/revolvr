#include "parser.h"

Parser::Parser(Scanner& scanner) : scanner_(scanner) {
  tokens_ = scanner_.GetTokens();
  ParseHeading();
}

void Parser::ParseHeading() {
  Heading heading;

  // Parse beginning bracket
  if (tokens_.front() != Token::BrackLeft)
    throw std::runtime_error("Heading should start with a bracket");
  tokens_.pop();

  // Parse heading type
  ParseHeadingType(heading);

  // Parse heading "key=value" pairs until we hit end bracket
  while(tokens_.front() != Token::BrackRight) {
    ParseHeadingKeyValuePairs(heading);
  }
}

void Parser::ParseField(std::vector<Field>& fields) {
  // While we haven't reached another heading
  while (tokens_.front() != Token::BrackLeft) {
    // All fields should begin with an identifier
    if (tokens_.front() != Token::Identifier) {
      std::string errMsg = "Expected identifer for field, got ";
      errMsg += Scanner::TokToString(tokens_.front()) + " instead";
    }
    std::string identifier = scanner_.GetNextIdentifier();
    tokens_.pop();

    // Begin parsing various fields
    Field field;
    field.fieldName = identifier;

    if (ParseField3(field)) {
      field.type = FieldType::Field3;
    }
    else if (ParseField4(field)) {
      field.type = FieldType::Field4;
    }
    else {
      std::string errMsg = "Unrecognized field. Token: " + Scanner::TokToString(tokens_.front());
      errMsg += ", field name: " + field.fieldName;
      throw std::runtime_error(errMsg);
    }
    fields.push_back(field);
  }
}

void Parser::ParseField1() {

}

void Parser::ParseField2() {

}

void Parser::PopulateList(float& f, bool skipComma) {
  if (tokens_.front() == Token::Number) {
    f = scanner_.GetNextNumber();
    tokens_.pop();
    if (tokens_.front() == Token::Comma && !skipComma) {
      tokens_.pop();
    }
    else {
      // missing comma
    }
  }
  else {
    // Missing number
  }
}

bool Parser::ParseField3(Field& field) {
  for (auto keyword : {"scale", "position"}) {
    if (field.fieldName == keyword) {
      // Check for left curly brace
      if (tokens_.front() == Token::BrackLeft) {
        // Here we should get '{' number ',' number ',' number '}'
        tokens_.pop(); // pop '{'
        PopulateList(field.vec3.x, false);
        PopulateList(field.vec3.y, false);
        PopulateList(field.vec3.z, true);
      }
      else {
        // error
      }

      // Check for right curly brace
      if (tokens_.front() == Token::BrackRight) {
        tokens_.pop();
      }
      else {
        // error
      }
      return true;
    }
  }
  return false;
}

bool Parser::ParseField4(Field& field) {

}

void Parser::ParseResource() {

}

void Parser::ParseHand() {

}

void Parser::PrintTokens() {
  while (!tokens_.empty()) {
    auto token = tokens_.front();
    if (token == Token::Identifier)
      std::cout << Scanner::TokToString(token) << ", " << scanner_.GetNextIdentifier() << "\n";
    else if (token == Token::Number)
      std::cout << Scanner::TokToString(token) << ", " << scanner_.GetNextNumber() << "\n";
    else
      std::cout << Scanner::TokToString(token) << "\n";
    tokens_.pop();
  }
}

std::vector<Parser::Unit>& Parser::GetUnits() {
  return units_;
}

void Parser::ParseHeadingType(Heading& heading) {
  if (tokens_.front() == Token::Identifier) {
    std::string identifier = scanner_.GetNextIdentifier();
    if (identifier == "node" || identifier == "resource") {
      heading.headingType = identifier;
    }
    else {
      throw std::runtime_error("Heading type " + identifier + " unrecognized");
    }
    tokens_.pop();
  }
  else {
    std::string errMsg = "Expected identifer for heading_type, got ";
    errMsg += Scanner::TokToString(tokens_.front()) + " instead";
    throw std::runtime_error(errMsg);
  }
}

void Parser::ParseHeadingKeyValuePairs(Heading& heading) {
  // First check for keys
  if (tokens_.front() == Token::Identifier) {
    std::string identifier = scanner_.GetNextIdentifier();
    // Number values
    if (identifier == "id" || identifier == "parent") {
      tokens_.pop();
      if (tokens_.front() == Token::Equals) {
        tokens_.pop();
        if (tokens_.front() == Token::Number) {
          heading.strKeyNumVal.push_back({identifier, scanner_.GetNextNumber()});
          tokens_.pop();
        }
      }
    }
    // String keys
    else if (identifier == "name" || identifier == "type") {
      tokens_.pop();
      if (tokens_.front() == Token::Equals) {
        tokens_.pop();
        if (tokens_.front() == Token::Identifier) {
          heading.strKeyStrVal.push_back({identifier, scanner_.GetNextIdentifier()});
          tokens_.pop();
        }
      }
    }
    else {
      throw std::runtime_error("Error while parsing header, key " + identifier + " unrecognized");
    }
  }
  else {
    std::string errMsg = "Expected identifer while parsing header keys, got ";
    errMsg += Scanner::TokToString(tokens_.front()) + " instead";
    throw std::runtime_error(errMsg);
  }
}