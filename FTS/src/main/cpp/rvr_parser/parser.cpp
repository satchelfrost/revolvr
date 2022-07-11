#include "parser.h"

Parser::Parser(Scanner& scanner) : scanner_(scanner) {
  ParseHeading();
}

void Parser::ParseHeading() {
  auto tokens = scanner_.GetTokens();
  Heading heading;

  // Check for beginning bracket
  if (tokens.front() != Token::BrackLeft)
    throw std::runtime_error("Heading should start with a bracket");
  tokens.pop();

  // Check for header type
  if (tokens.front() == Token::Identifier) {
    std::string identifier = scanner_.GetNextIdentifier();
    if (identifier == "node" || identifier == "resource") {
      heading.headingType = identifier;
    }
    else {
      throw std::runtime_error("Heading type " + identifier + " unrecognized");
    }
    tokens.pop();
  }
  else {
    std::string errMsg = "Expected identifer for heading_type, got ";
    errMsg += Scanner::TokToString(tokens.front()) + " instead";
    throw std::runtime_error(errMsg);
  }

  // Check for heading "key=value" pairs
  while(tokens.front() != Token::BrackRight) {
    // First check for keys
    if (tokens.front() == Token::Identifier) {
      std::string identifier = scanner_.GetNextIdentifier();
      // Number values
      if (identifier == "id" || identifier == "parent") {
        tokens.pop();
        if (tokens.front() == Token::Equals) {
          tokens.pop();
          if (tokens.front() == Token::Number) {
            heading.strKeyNumVal.push_back({identifier, scanner_.GetNextNumber()});
            tokens.pop();
          }
        }
      }
      // String keys
      else if (identifier == "name" || identifier == "type") {
        tokens.pop();
        if (tokens.front() == Token::Equals) {
          tokens.pop();
          if (tokens.front() == Token::Identifier) {
            heading.strKeyStrVal.push_back({identifier, scanner_.GetNextIdentifier()});
            tokens.pop();
          }
        }
      }
      else {
        throw std::runtime_error("Error while parsing header, key " + identifier + " unrecognized");
      }
    }
    else {
      std::string errMsg = "Expected identifer while parsing header keys, got ";
      errMsg += Scanner::TokToString(tokens.front()) + " instead";
      throw std::runtime_error(errMsg);
    }
  }
}

void Parser::ParseField() {

}

void Parser::ParseField1() {

}

void Parser::ParseField2() {

}

void Parser::ParseField3() {

}

void Parser::ParseField4() {

}

void Parser::ParseResource() {

}

void Parser::ParseHand() {

}

void Parser::PrintTokens() {
  auto tokens = scanner_.GetTokens();
  while (!tokens.empty()) {
    auto token = tokens.front();
    if (token == Token::Identifier)
      std::cout << Scanner::TokToString(token) << ", " << scanner_.GetNextIdentifier() << "\n";
    else if (token == Token::Number)
      std::cout << Scanner::TokToString(token) << ", " << scanner_.GetNextNumber() << "\n";
    else
      std::cout << Scanner::TokToString(token) << "\n";
    tokens.pop();
  }
}

std::vector<Parser::Unit>& Parser::GetUnits() {
  return units_;
}

    // bool SetIfField1(std::string compare) {
    //   for (auto keyword : {"visible", "custom"}) {
    //     if (compare == keyword) {
    //         key = keyword;
    //         return true;
    //     }
    //   }
    //   return false;
    // }