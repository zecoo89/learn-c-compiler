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
      return node->name;
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
    case ND_BLOCK:
      return "BLOCK";
    case ND_CALL:
      return node->name;
    case ND_DEF:
      return node->name;
    default:
      return "";
  }
}

void print_node_tree(FILE *f, Node *node) {
  int node_id = max_node_id;

  if(node->kind == ND_DEF) {
    fprintf(f,"node_%d[label=\"define %s(args_len=%d)\"]\n", node_id, get_node_kind_string(node), node->args_len);
    max_node_id++;
    fprintf(f,"node_%d -> node_%d[style=\"dashed\"]\n", node_id, max_node_id);
    node_id = max_node_id;
    node = node->stmt;
  }

  if(node->stmt) {
    fprintf(f,"node_%d[label=\"%s\"]\n", node_id, get_node_kind_string(node));
    max_node_id++;
    fprintf(f,"node_%d -> node_%d[style=\"dashed\"]\n", node_id, max_node_id);
    print_node_tree(f, node->stmt);
  }

  // parent node
  if (node->kind == ND_CALL) {
    fprintf(f,"node_%d[label=\"call %s|{", node_id, get_node_kind_string(node));
    for(int i=0;i<node->args_len;i++) {
      fprintf(f, "<arg%d>arg:%d", i, i);
    }
    fprintf(f, "}\", shape=\"record\"]\n");
    max_node_id++;
    if(node->args_len > 0) {
      for(int i=0;i<node->args_len;i++) {
        fprintf(f, "node_%d:arg%d -> node_%d\n", node_id, i, max_node_id);
        print_node_tree(f, node->args[i]);
        max_node_id++;
      }
    }
  } else if(node->kind == ND_IF) {
    if(node->els) {
      fprintf(f,"node_%d[label=\"<if>%s|<cond>cond|<then>then|<else>else\",shape=\"record\"]\n", node_id, get_node_kind_string(node));
    } else {
      fprintf(f,"node_%d[label=\"<if>%s|<cond>cond|<then>then\",shape=\"record\"]\n", node_id, get_node_kind_string(node));
    }
    max_node_id++;
    fprintf(f,"node_%d:cond -> node_%d\n", node_id, max_node_id);
    print_node_tree(f, node->cond);
    max_node_id++;
    fprintf(f,"node_%d:then -> node_%d\n", node_id, max_node_id);
    print_node_tree(f, node->then);
    if(node->els) {
      max_node_id++;
      fprintf(f,"node_%d:else -> node_%d\n", node_id, max_node_id);
      print_node_tree(f, node->els);
    }
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
  fprintf(f, "  graph [\n    charset = \"UTF-8\"\n    rankdir=\"LR\"\n  ];\n");
}

void print_node_tree_end(FILE *f) {
  fprintf(f, "}\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = read_file(argv[1]);
  tokenize();
  parse();
  print_node_tree_init(stdout);
  for(int i=0;code[i];i++) {
    print_node_tree(stdout, code[i]);
    max_node_id++;
  }

  print_node_tree_end(stdout);
  return 0;
}
