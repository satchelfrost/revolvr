#include "scanner.h"
#include "parser.h"

int main() {
  Scanner scanner("../example.rvr");
  Parser parser(scanner);
  // parser.PrintTokens();
}