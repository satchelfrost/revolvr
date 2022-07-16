#include "scanner.h"
#include "parser.h"

int main() {
  Parser parser("../example.rvr");
  parser.Parse();
  // parser.PrintTokens();
}
