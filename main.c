#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char *user_input;
Token *token;
Node *node;
Node *code[100];
LVar *locals;

extern bool at_eof();

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);

  //fprintf(stderr, "tokenized\n");
  /*
  Token *head = token;
  while(!at_eof()) {
    fprintf(stderr, "%s\n", token->str);
    token = token->next;
  }

  token = head;
  /**/

  parse();
  //fprintf(stderr, "parsed\n");

  // アセンブリの最初の部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf(".globl _main\n");
  printf("main:\n");
  printf("_main:\n");

  //プロローグ
  //変数26個分の領域を確保する
  printf("# prologue\n");
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    printf("# end_line\n");
    printf("  pop rax\n");
  }

  printf("# epilogue\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
