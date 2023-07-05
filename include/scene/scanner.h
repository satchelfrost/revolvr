/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include "pch.h"
#include "check.h"
#include "platform/android_context.h"

#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "token.h"
#include <iostream>
#include <sstream>
#include <android/asset_manager.h>

namespace rvr {
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
}