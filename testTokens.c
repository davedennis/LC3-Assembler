#include <stdio.h>

#include "tokens.h"

int main (int argc, char* argv[]) {
  if (argc == 2) {
    tokens_init();
    int i = 0;

    for (char* tok = tokenize_line(argv[1]); tok != NULL; tok = next_token()) {
      printf("token[%d]: = '%s'\n", i++, tok);
    }

    tokens_term();
    return 0;
  }

  printf("Usage: testTokens 'line to tokenize' (must include single quotes)\n");
  printf("e.g. testTokens 'LABEL ADD R1,R2,#5; comment'\n");
  return 1;
}
