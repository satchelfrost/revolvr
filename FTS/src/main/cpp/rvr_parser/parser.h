#pragma once

#include "pch.h"
#include "check.h"
#include "logger.h"
#include "token.h"
#include "scanner.h"
#include <map>

class Parser {
public:
  Parser(const std::string& fileName);
  void PrintTokens();

  enum FieldType {
    // names based on grammar definition
    Field1 = 0,
    Field2 = 1,
    Field3 = 2,
    Field4 = 3,
    Resource = 4,
    Hand = 5
  };

  struct Heading {
    std::string headingType;
    std::map<std::string, std::string> strKeyStrVal;
    std::map<std::string, int> strKeyNumVal;
  };

  struct Field {
    FieldType type;
    std::string fieldName;
    union {
      XrQuaternionf quat;
      XrVector3f vec3;
      XrVector2f vec2;
      int handSide; // left 0, right 1
      int resource_id;
      bool boolean;
    };
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
  std::vector<Field> ParseFields();
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