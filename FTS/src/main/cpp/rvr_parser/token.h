#include <string>

enum class Token {
  BrackLeft = 0,
  BrackRight = 1,
  Identifier = 2,
  Comma = 3,
  CurlLeft = 4,
  CurlRight = 5,
  Number = 6,
  Equals = 7,
};

inline std::string to_string(Token t) {
  switch (t) {
  case Token::BrackLeft:
    return "BrackLeft";
  case Token::BrackRight:
    return "BrackRight";
  case Token::Identifier:
    return "Identifier";
  case Token::Comma:
    return "Comma";
  case Token::CurlLeft:
    return "CurlLeft";
  case Token::CurlRight:
    return "CurlRight";
  case Token::Number:
    return "Number";
  case Token::Equals:
    return "Equals";
  default:
    return "Token \"" + std::to_string((int)t) + "\" unrecognized";
  }
}