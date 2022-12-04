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
  void PrintTokens();

  enum FieldValueType {
      Number = 0,
      String = 1
  };

  struct Heading {
    std::string headingType;
    std::map<std::string, std::string> strKeyStrVal;
    std::map<std::string, int> strKeyNumVal;
  };

  struct Any {
      std::string typeName;
      void* any;
  };

  struct Access {
      std::string accessName;
      union {
          Access* access;
          float floatValue;
          bool boolValue;
          Any* any;
      };
  };

  struct Field {
      rvr::ComponentType cType;
      Access* access;
  };

  struct Unit {
    Heading heading;
    std::vector<Field> fields;
  };

private:

  std::vector<Unit> units_;

  // Parse methods
  void ParseHeading(Heading& heading);
  void ParseHeadingType(Heading& heading);
  void ParseHeadingKeyValuePairs(Heading& heading);
  void ParseAccess(Access* access);
  std::vector<Field> ParseFields();
  Field ParseField();
  bool ParseField1(Field& field);
  bool ParseField2(Field& field);
  bool ParseField3(Field& field);
  bool ParseField4(Field& field);
  bool ParseResourceId(Field& field);
  bool ParseHand(Field& field);
  void ReadCurlyList(float& number, bool commaExpected);
  void ReadBool(bool& b);
  void ReadSide(int& side);

  Token::Tok Peek();
  Token Pop();
  Token prevToken_;
  Scanner scanner_;
  std::string fileName_;
  std::queue<Token> tokens_;

  // Error & Check methods
  void ParseErrorPrevToken(const std::string& errMsg);
  void TokenError(std::string errMsg, Token::Tok expected);
  void CheckPeek(const char* errMsg, Token::Tok expected);
  void CheckPop(const char* errMsg, Token::Tok expected);

public:
  std::vector<Unit> Parse();
};
}