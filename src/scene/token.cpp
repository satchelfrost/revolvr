/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <scene/token.h>
#include <utility>

Token::Token() : tok(Blank) {}

Token::Token(const Token& t) : tok(t.tok), line_(t.line_), column_(t.column_) {
  switch(t.tok) {
    case Number:
      number_ = t.number_;
      break;
    case Identifier:
      identifier_ = t.identifier_;
      break;
    default:
      break;
  }
}

Token& Token::operator=(const Token& t) {
  tok = t.tok;
  line_ = t.line_;
  column_ = t.column_;

  switch(t.tok) {
    case Number:
      number_ = t.number_;
      break;
    case Identifier:
      identifier_ = t.identifier_;
      break;
    default:
      break;
  }
  return *this;
}

Token::Token(Tok tokP, int line, int column) :
tok(tokP), line_(line), column_(column) {}

Token::Token(Tok tokP, int line, int column, double number) :
tok(tokP), line_(line), column_(column), number_(number) {
  if (tok != Tok::Number)
    throw std::runtime_error("Number token expected for construction.");
}

Token::Token(Tok tokP, int line, int column, std::string identifier) :
tok(tokP), line_(line), column_(column), identifier_(std::move(identifier)) {
  if (tok != Identifier)
    throw std::runtime_error("Identifier token expected for construction.");
}

double Token::GetNumber() const {
  if (tok != Number)
    THROW(Fmt("Cannot get number from %s", StringFromTokEnum(tok).c_str()));
  return number_;
}

std::string Token::GetIdentifier() {
  if (tok != Identifier)
    THROW(Fmt("Cannot get identifier from %s", StringFromTokEnum(tok).c_str()));
  return identifier_;
}

std::string Token::StringFromTokEnum(Tok token) {
  switch (token) {
    case BrackLeft:
      return "Left Bracket";
    case BrackRight:
      return "Right Bracket";
    case Identifier:
      return "Identifier";
    case Comma:
      return "Comma";
    case CurlLeft:
      return "Left Curly Brace";
    case CurlRight:
      return "Right Curly Brace";
    case Number:
      return "Number";
    case Equals:
      return "Equals";
    case Dot:
      return "Dot";
    case Comment:
      return "Comment";
    default:
      return "Token \"" + std::to_string((int)token) + "\" unrecognized";
  }
}

std::string Token::ToString() {
  switch (tok) {
    case BrackLeft:
      return "[";
    case BrackRight:
      return "]";
    case Identifier:
      return GetIdentifier();
    case Comma:
      return ",";
    case CurlLeft:
      return "{";
    case CurlRight:
      return "}";
    case Number:
      return std::to_string(GetNumber());
    case Equals:
      return "=";
    case Comment:
      return "#";
    case Dot:
      return ".";
    default:
      return "Token \"" + std::to_string((int)tok) + "\" unrecognized";
  }
}

std::string Token::LineColString() const {
  return "[line: " + std::to_string(line_) + ", column: " + std::to_string(column_) + "]";
}