#include "scanner.h"

Scanner::Scanner(std::string fileName) : currentLine_(1) {
  inputStream_ = std::ifstream(fileName);
  if (!inputStream_)
    std::cerr << "Cannot open file.\n";

  ReadLines();
  for (auto line : lines_) {
    Tokenize(line);
    currentLine_++;
  }
}

void Scanner::ReadLines() {
  std::string line;
  while(getline(inputStream_, line))
    lines_.push_back(line);
}

bool Scanner::IdentifierScan(std::string& line, int& pos) {
  if(isalpha(line[pos]) || line[pos] == '_') {
    std::string identifier;
    do {
      if(isalnum(line[pos]) || line[pos] == '_') {
        identifier += line[pos];
      }
      else {
        pos--;
        break;
      }
    } while(pos++ < line.size());
    tokens_.push({Tok::Identifier, currentLine_, pos + 1});
    identifierQueue_.push(identifier);
    return true;
  }
  return false;
}

bool Scanner::NumberScan(std::string& line, int& pos) {
  if (isdigit(line[pos]) || line[pos] == '.' || line[pos] == '-') {
    std::string numStr;
    numStr += line[pos];
    while(pos++ < line.size()) {
      if(isdigit(line[pos]) || line[pos] == '.') {
        numStr += line[pos];
      }
      else {
        pos--;
        break;
      }
    } 
    tokens_.push({Tok::Number, currentLine_, pos + 1});
    numberQueue_.push(strtod(numStr.c_str(), nullptr));
    return true;
  }
  return false;
}

void Scanner::Tokenize(std::string line) {
  // Iterate throught the characters in a line
  for (int pos = 0; pos < line.size(); pos++) {
    // Skip over whitespace
    if(isspace(line[pos]))
      continue;

    // Check for identifiers
    if(IdentifierScan(line, pos))
      continue;

    // Check for Numbers
    if(NumberScan(line, pos))
      continue;

    // Check for remaining characters
    switch (line[pos]) {
      case '[':
        tokens_.push({Tok::BrackLeft, currentLine_, pos + 1});
        break;
      case ']':
        tokens_.push({Tok::BrackRight, currentLine_, pos + 1});
        break;
      case ',':
        tokens_.push({Tok::Comma, currentLine_, pos + 1});
        break;
      case '{':
        tokens_.push({Tok::CurlLeft, currentLine_, pos + 1});
        break;
      case '}':
        tokens_.push({Tok::CurlRight, currentLine_, pos + 1});
        break;
      case '=':
        tokens_.push({Tok::Equals, currentLine_, pos + 1});
        break;
      default:
        std::cerr << "Token unrecognized: \"" << line[pos] << "\"\n";
    }
  }
}

std::string Scanner::GetNextIdentifier() {
  auto front = identifierQueue_.front();
  identifierQueue_.pop();
  return front;
}

double Scanner::GetNextNumber() {
  auto front = numberQueue_.front();
  numberQueue_.pop();
  return front;
}

std::string Scanner::TokToString(Tok t) {
  switch (t) {
  case Tok::BrackLeft:
    return "BrackLeft";
  case Tok::BrackRight:
    return "BrackRight";
  case Tok::Identifier:
    return "Identifier";
  case Tok::Comma:
    return "Comma";
  case Tok::CurlLeft:
    return "CurlLeft";
  case Tok::CurlRight:
    return "CurlRight";
  case Tok::Number:
    return "Number";
  case Tok::Equals:
    return "Equals";
  default:
    return "Token \"" + std::to_string((int)t) + "\" unrecognized";
  }
}