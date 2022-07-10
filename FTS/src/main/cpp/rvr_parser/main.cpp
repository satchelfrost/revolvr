#include "scanner.h"

int main() {
  Scanner s("../example.rvr");
  for (auto token : s.GetTokens()) {
    if (token == Token::Identifier)
      std::cout << to_string(token) << ", " << s.GetNextIdentifier() << "\n";
    else if (token == Token::Number)
      std::cout << to_string(token) << ", " << s.GetNextNumber() << "\n";
    else
      std::cout << to_string(token) << "\n";
  }
}