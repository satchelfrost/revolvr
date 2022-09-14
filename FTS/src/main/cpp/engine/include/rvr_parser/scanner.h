#pragma once

#include "pch.h"
#include "check.h"
#include "platform/rvr_android_context.h"

#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "token.h"
#include <iostream>
#include <sstream>
#include <android/asset_manager.h>

class Scanner {
public:
  Scanner(const std::string& fileName);
  void ReadLines();
  void Tokenize(std::string line);
  bool IdentifierScan(std::string& line, int& pos);
  bool NumberScan(std::string& line, int& pos);
  std::queue<Token> GetTokens() { return tokens_; }
private:
  std::vector<std::string> lines_;
  std::istringstream inputStream_;
  std::queue<Token> tokens_;
  int currentLine_;
};