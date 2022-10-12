#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char *user_input;
Token *token;
Node *node;
Node *code[100];
LVar *locals;
int max_node_id = 0;

extern bool at_eof();

void print_token(FILE *f,char *p, int len) {
  char result[100];
  strncpy(result, p, len);
  result[len] = '\0';
  fprintf(f, "[%s]", result);
}

void print_tokens(FILE *f) {
  printf("--- tokens ---\n");
  Token *head = token;

  while(!at_eof()) {
    print_token(f, token->str, token->len);
    token = token->next;
  }

  fprintf(f, "\n");

  token = head;

  printf("--- end ---\n");
}

void print_arrow(FILE *f, Token *token, int node_id) {
  Token *token1 = token;
  Token *token2 = token->next;
  char result1[100];
  char result2[100];
  int len1 = token1->len;
  int len2 = token2->len;

  strncpy(result1, token1->str, token1->len);
  strncpy(result2, token2->str, token2->len);
  result1[len1] = '\0';
  result2[len2] = '\0';

  fprintf(f, "  node%d[label=\"%s\"]\n", node_id, result1);
  fprintf(f, "  node%d[label=\"%s\"]\n", node_id+1, result2);
  fprintf(f, "  node%d -> node%d\n", node_id, node_id+1);
}

void print_token_graph(FILE *f) {
  fprintf(f, "digraph tokens {\n");
  fprintf(f, "  graph [\n    charset = \"UTF-8\"\n    rankdir=\"TB\"\n  ];\n");

  Token *head = token;
  int node_id = 0;
  while(!at_eof()) {
    if(token->next->kind == TK_EOF)
      break;

    print_arrow(f, token, node_id);
    token = token->next;
    node_id += 1;
  }

  token = head;

  fprintf(f, "}\n");
  exit(0);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = read_file(argv[1]);
  token = tokenize();

  //print_tokens(stdout);
  print_token_graph(stdout);

  return 0;
}
