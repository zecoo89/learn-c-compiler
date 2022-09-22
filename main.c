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
  fprintf(f, "  graph [\n    charset = \"UTF-8\"\n    rankdir = LR\n  ];\n");

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

char *get_node_kind_string(Node *node){
  char* num_str = malloc(sizeof(char) * 20);

  switch(node->kind) {
    case ND_ADD:
      return "+";
    case ND_SUB:
      return "-";
    case ND_MUL:
      return "*";
    case ND_DIV:
      return "/";
    case ND_FOR:
      return "for";
    case ND_NUM:
      sprintf(num_str, "%d", node->val);
      return num_str;
    case ND_LVAR:
      return node->var_name;
    case ND_IF:
      return "if";
    case ND_WHILE:
      return "while";
    case ND_EQ:
      return "==";
    case ND_LT:
      return "<";
    case ND_LE:
      return "<=";
    case ND_ASSIGN:
      return "=";
    case ND_RETURN:
      return "return";
    default:
      return "";
  }
}

void print_node_tree(FILE *f, Node *node) {
  int node_id = max_node_id;
  // parent node
  if(node->kind == ND_IF) {
    fprintf(f,"node_%d[label=\"<if>%s|<cond>cond|<then>then\",shape=\"record\"]\n", node_id, get_node_kind_string(node));
    max_node_id++;
    fprintf(f,"node_%d:cond -> node_%d\n", node_id, max_node_id);
    print_node_tree(f, node->cond);
    max_node_id++;
    fprintf(f,"node_%d:then -> node_%d\n", node_id, max_node_id);
    print_node_tree(f, node->then);
  } else if(node->kind == ND_WHILE) {
    fprintf(f,"node_%d[label=\"<while>%s|<cond>cond|<body>body\",shape=\"record\"]\n", node_id, get_node_kind_string(node));
      max_node_id++;
      fprintf(f,"node_%d:cond -> node_%d\n", node_id, max_node_id);
      print_node_tree(f, node->cond);
      max_node_id++;
      fprintf(f,"node_%d:body -> node_%d\n", node_id, max_node_id);
      print_node_tree(f, node->body);
  } else {
    fprintf(f,"node_%d[label=\"%s\"]\n", node_id, get_node_kind_string(node));
    //left child
    if(node->lhs) {
      max_node_id++;
      fprintf(f,"node_%d -> node_%d\n", node_id, max_node_id);
      print_node_tree(f, node->lhs);
    }
    //right child
    if(node->rhs) {
      max_node_id++;
      fprintf(f,"node_%d -> node_%d\n", node_id, max_node_id);
      print_node_tree(f, node->rhs);
    }
  }
}

void print_node_tree_init(FILE *f) {
  fprintf(f, "digraph tokens {\n");
  fprintf(f, "  graph [\n    charset = \"UTF-8\"\n    rankdir = LR\n  ];\n");
}

void print_node_tree_end(FILE *f) {
  fprintf(f, "}\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);

  //print_tokens(stdout);
  //print_token_graph(stdout);
  parse();
  print_node_tree_init(stdout);
  for(int i=0;code[i];i++) {
    print_node_tree(stdout, code[i]);
    max_node_id++;
  }

  print_node_tree_end(stdout);
  exit(0);

  // アセンブリの最初の部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf(".globl _main\n"); // for Mac support
  printf("main:\n");
  printf("_main:\n"); // for Mac support

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
