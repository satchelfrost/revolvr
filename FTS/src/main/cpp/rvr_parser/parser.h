#pragma once

#include "token.h"
#include "scanner.h"

class Parser {
public:
  Parser(Scanner& scanner);
  void PrintTokens();

private:
  void ParseHeading();

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
    std::vector<std::pair<std::string, std::string>> strKeyStrVal;
    std::vector<std::pair<std::string, double>> strKeyNumVal;
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

  struct Field {
    FieldType type;
    std::string fieldName;
    bool value;
    XrVector2f vec2;
    XrVector3f vec3;
    XrQuaternionf quat;
    int id;
    int side; // left 0, right 1
  };

  struct Unit {
    Heading heading;
    std::vector<Field> fields;
  };

  std::vector<Unit> units_;
  Scanner& scanner_;
  std::queue<Token> tokens_;

  void ParseHeadingType(Heading& heading);
  void ParseHeadingKeyValuePairs(Heading& heading);
  void ParseField(std::vector<Field>& fields);
  void ParseField1();
  void ParseField2();
  bool ParseField3(Field& field);
  bool ParseField4(Field& field);
  void ParseResource();
  void ParseHand();
  void PopulateList(float& f, bool skipComma);
public:
  std::vector<Unit>& GetUnits();
};