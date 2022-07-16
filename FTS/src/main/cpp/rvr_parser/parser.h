#pragma once

#include "token.h"
#include "scanner.h"
#include <map>

class Parser {
public:
  // Parser(Scanner& scanner);
  Parser(std::string fileName);
  void Parse();
  void PrintTokens();

private:

  // Manually define openxr stuff until I can get the project fully linked
  typedef struct XrVector2f {
      float    x;
      float    y;
  } XrVector2f;

  typedef struct XrVector3f {
      float    x;
      float    y;
      float    z;
  } XrVector3f;

  typedef struct XrQuaternionf {
      float    x;
      float    y;
      float    z;
      float    w;
  } XrQuaternionf;

  struct Heading {
    std::string headingType;
    std::map<std::string, std::string> strKeyStrVal;
    std::map<std::string, int> strKeyNumVal;
  };

  enum class FieldType {
    // names based on grammar definition
    Field1 = 0,
    Field2 = 1,
    Field3 = 2,
    Field4 = 3,
    Resource = 4,
    Hand = 5
  };

  // There are multiple field types and they will not fill up this struct alone
  struct Field {
    FieldType type;
    std::string fieldName;
    bool visible;
    bool customType;
    XrVector2f vec2;
    XrVector3f vec3;
    XrQuaternionf quat;
    int hand; // left 0, right 1
    int resource;
  };

  struct Unit {
    Heading heading;
    std::vector<Field> fields;
  };

  std::vector<Unit> units_;
  // Scanner& scanner_;
  Scanner scanner_;
  std::string fileName_;
  std::queue<Token> tokens_;

  void ParseHeading(Heading& heading);
  void ParseHeadingType(Heading& heading);
  void ParseHeadingKeyValuePairs(Heading& heading);
  std::vector<Field> ParseFields();
  bool ParseField1(Field& field);
  bool ParseField2(Field& field);
  bool ParseField3(Field& field);
  bool ParseField4(Field& field);
  bool ParseResource(Field& field);
  bool ParseHand(Field& field);
  void ReadCurlyList(float& f, bool commaExpected);
  void ReadBool(bool& b);
  void ReadSide(int& side);

  Tok Peek();
  void Pop();
  std::string LineColString();
  void ParseError(std::string errMsg);
  Token lastToken_;

public:
  std::vector<Unit>& GetUnits();
};