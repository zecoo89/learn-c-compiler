#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char *user_input;
Token *token;
Node *node;
Node *func_head;
Node *func_tail;
Node *code[100];
LVar *locals;
int max_node_id = 0;

extern bool at_eof();

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = read_file(argv[1]);
  tokenize();
  parse();

  gen_prologue();
  gen_codes(code);
  return 0;
}
