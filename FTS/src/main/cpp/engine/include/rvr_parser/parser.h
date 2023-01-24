#pragma once

#include "pch.h"
#include "check.h"
#include "logger.h"
#include "token.h"
#include "scanner.h"
#include "include/ecs/component/component_type.h"
#include <map>

namespace rvr {
class Parser {
public:
  Parser(const std::string& fileName);

  struct Heading {
    std::string headingType;
    std::map<std::string, std::string> strKeyStrVal;
    std::map<std::string, int> strKeyNumVal;
  };

  struct Field {
      rvr::ComponentType cType;
      std::string fullyQualifiedName;
      std::vector<std::string> strValues;
      std::vector<float> floatValues;
  };

  struct Unit {
    Heading heading;
    std::map<ComponentType, std::map<std::string, Field>> fields;
  };

private:
  std::vector<Unit> units_;

  // helper methods
  void ParseHeading(Heading& heading);
  void ParseHeadingType(Heading& heading);
  void ParseHeadingKeyValuePairs(Heading& heading);
  void ParseAccess(Field& field);
  void ParseAccessStrValues(Field& field);
  void ParseAccessFloatValues(Field& field);
  std::map<ComponentType, std::map<std::string, Field>> ParseFields();
  Field ParseField();
  Token::Tok Peek();
  Token Pop();

  // Error & Check methods
  void ParseErrorPrevToken(const std::string& errMsg);
  void TokenError(std::string errMsg, Token::Tok expected);
  void CheckPeek(const char* errMsg, Token::Tok expected);
  void CheckPop(const char* errMsg, Token::Tok expected);

  Token prevToken_;
  Scanner scanner_;
  std::queue<Token> tokens_;

public:
  std::vector<Unit> Parse();
};
}