#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "token.h"
#include <iostream>

class Scanner {
public:
  Scanner(std::string fileName);
  void ReadLines();
  void Tokenize(std::string line);
  bool IdentifierScan(std::string& line, int& pos);
  bool NumberScan(std::string& line, int& pos);
  std::queue<Token> GetTokens() { return tokens_; }
private:
  std::vector<std::string> lines_;
  std::ifstream inputStream_;
  std::queue<Token> tokens_;
  int currentLine_;
};