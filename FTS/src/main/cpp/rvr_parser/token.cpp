#include "token.h"

Token::Token() : tok(Blank) {}

Token::Token(const Token& t) : tok(t.tok), line_(t.line_), column_(t.column_) {
  switch(t.tok) {
    case Number:
      number_ = t.number_;
      break;
    case Identifier:
      identifier_ = t.identifier_;
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
tok(tokP), line_(line), column_(column), identifier_(identifier) {
  if (tok != Identifier)
    throw std::runtime_error("Identifier token expected for construction.");
}

double Token::GetNumber() {
  if (tok != Number)
    throw std::runtime_error("Cannot get number from " + TokToString(tok));
  return number_;
}

std::string Token::GetIdentifier() {
  if (tok != Identifier)
    throw std::runtime_error("Cannot get identifier from " + TokToString(tok));
  return identifier_;
}

std::string Token::TokToString(Tok token) {
  switch (token) {
  case BrackLeft:
    return "[";
  case BrackRight:
    return "]";
  case Identifier:
    return "Identifier";
  case Comma:
    return ",";
  case CurlLeft:
    return "{";
  case CurlRight:
    return "}";
  case Number:
    return "Number";
  case Equals:
    return "=";
  default:
    return "Token \"" + std::to_string((int)token) + "\" unrecognized";
  }
}

std::string Token::ToString() {
  return TokToString(tok);
}

std::string Token::LineColString() {
  return "[line: " + std::to_string(line_) + ", column: " + std::to_string(column_) + "]";
}