#pragma once
#include <iostream>
#include <string>
#include "common.h"

class Token {
public:
  enum Tok {
    BrackLeft = 0,
    BrackRight = 1,
    Identifier = 2,
    Comma = 3,
    CurlLeft = 4,
    CurlRight = 5,
    Number = 6,
    Equals = 7,
    Blank = 8
  };
  Token& operator=(const Token& token);
  Token(const Token& token);
  Token();
  Token(Tok tok, int line, int column);
  Token(Tok tok, int line, int column, double number);
  Token(Tok tok, int line, int column, std::string identifier);
  Tok tok;
  std::string ToString();
  double GetNumber();
  std::string GetIdentifier();
  static std::string StringFromTokEnum(Tok token);
  std::string LineColString() const;

private:
  int line_;
  int column_;
  double number_;          // for number tokens only
  std::string identifier_; // for identifiers tokens only
};
