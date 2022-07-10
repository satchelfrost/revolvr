#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "token.h"

// The following headers are only useful for testing
#include <iostream>

class Scanner {
public:
  Scanner(std::string fileName);
  void ReadLines();
  void Tokenize(std::string line);
  bool IdentifierScan(std::string& line, int& pos);
  bool NumberScan(std::string& line, int& pos);

  std::vector<Token>& GetTokens() { return tokens_; }
  std::string GetNextIdentifier();
  double GetNextNumber();

private:
  std::vector<std::string> lines_;
  std::ifstream inputStream_;
  std::vector<Token> tokens_;
  std::queue<std::string> identifierQueue_;
  std::queue<double> numberQueue_;
  int currentLine_;
};