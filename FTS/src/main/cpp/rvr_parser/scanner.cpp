#include "scanner.h"

Scanner::Scanner(const std::string& fileName) : currentLine_(1) {
  Log::Write(Log::Level::Info, Fmt("Preparing to load asset"));
  AAsset *file = AAssetManager_open(RVRAndroidPlatform::GetInstance()->GetAndroidAssetManager(),
                                    fileName.c_str(),
                                    AASSET_MODE_BUFFER);
  off_t file_length = AAsset_getLength(file);
  char *file_content = new char[file_length + 1];
  AAsset_read(file, file_content, file_length);
  file_content[file_length] = '\0';

  inputStream_ = std::istringstream(file_content);
  if (!inputStream_)
    THROW(Fmt("Cannot open file %s", fileName.c_str()));

  Log::Write(Log::Level::Info, Fmt("loaded file %s", fileName.c_str()));
  ReadLines();
  for (const auto& line : lines_) {
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
    tokens_.push({Token::Identifier, currentLine_, pos + 1, identifier});
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
    double number = strtod(numStr.c_str(), nullptr);
    tokens_.push({Token::Number, currentLine_, pos + 1, number});
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
      default:
        std::cerr << "Token unrecognized: \"" << line[pos] << "\"\n";
    }
  }
}