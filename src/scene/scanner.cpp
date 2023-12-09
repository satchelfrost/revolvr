/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <scene/scanner.h>
#include <global_context.h>

namespace rvr {
Scanner::Scanner(const std::string& fileName) : currentLine_(1) {
  AAsset *file = AAssetManager_open(GlobalContext::Inst()->GetAndroidContext()->GetAndroidAssetManager(),
                                    fileName.c_str(),
                                    AASSET_MODE_BUFFER);
  off_t file_length = AAsset_getLength(file);
  char *file_content = new char[file_length + 1];
  AAsset_read(file, file_content, file_length);
  AAsset_close(file);
  file_content[file_length] = '\0';

  inputStream_ = std::istringstream(file_content);
  if (!inputStream_)
    THROW(Fmt("Cannot open file %s", fileName.c_str()));

  ReadLines();
  for (const auto& line : lines_) {
    Tokenize(line);
    currentLine_++;
  }

  delete[] file_content;
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
      if(isalnum(line[pos]) || line[pos] == '_' || line[pos] == '/') {
        identifier += line[pos];
      }
      else {
        pos--;
        break;
      }
    } while(pos++ < line.size());
    tokens_.push({Token::Identifier, currentLine_, pos + 1, identifier});
    return true;
  }
  return false;
}

bool Scanner::NumberScan(std::string& line, int& pos) {
  if (isdigit(line[pos]) || line[pos] == '-') {
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
    double number = strtod(numStr.c_str(), nullptr);
    tokens_.push({Token::Number, currentLine_, pos + 1, number});
    return true;
  }
  return false;
}

void Scanner::Tokenize(std::string line) {
  // Iterate throughout the characters in a line
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
        tokens_.push({Token::BrackLeft, currentLine_, pos + 1});
        break;
      case ']':
        tokens_.push({Token::BrackRight, currentLine_, pos + 1});
        break;
      case ',':
        tokens_.push({Token::Comma, currentLine_, pos + 1});
        break;
      case '{':
        tokens_.push({Token::CurlLeft, currentLine_, pos + 1});
        break;
      case '}':
        tokens_.push({Token::CurlRight, currentLine_, pos + 1});
        break;
      case '=':
        tokens_.push({Token::Equals, currentLine_, pos + 1});
        break;
      case '.':
        tokens_.push({Token::Dot, currentLine_, pos + 1});
        break;
      case '#':
        tokens_.push({Token::Comment, currentLine_, pos + 1});
        return; // return here because comments ignore any further characters
      default:
        std::cerr << "Token unrecognized: \"" << line[pos] << "\"\n";
    }
  }
}
}