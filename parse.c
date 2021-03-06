#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

extern Token *token;
extern LVar *locals;
extern void error_at(char *, char *, ...);

Node *new_node();
Node *new_node_num();

bool at_eof();

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

LVar *find_lvar(Token *);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;

  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;

  return node;
}

// エラーを報告する関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 次のトークンが期待している記号のときには、トークンを一つ読み進め、
// 真を返す、それ以外は偽を返す。
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    return false;
  }

  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind == TK_IDENT) {
    Token *tok = token;
    token = token->next;
    return tok;
  }

  return NULL;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
      error("'%s'ではありません", op);
  }

  token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM) {
    fprintf(stderr, "%d\n", TK_NUM);
    fprintf(stderr, "%d\n", token->kind);
    error_at(token->str, "数ではありません");
  }

  int val = token->val;
  token = token->next;

  return val;
}

void parse() {
  program();
}

extern Node *code[];
void program() {
  int i = 0;

  while (!at_eof()) {
    code[i++] = stmt();
  }

  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if(token->kind == TK_RETURN) {
    token = token->next;
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else if(token->kind == TK_IF) {

  } else {
    node = expr();
  }

  expect(";");

  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  //fprintf(stderr, "assign\n");
  Node *node = equality();

  for(;;) {
    if(consume("=")) {
      node = new_node(ND_ASSIGN, node, assign());
    } else {
      return node;
    }
  }

  return node;
}

Node *equality() {
  //fprintf(stderr, "equality\n");
  Node *node = relational();

  for(;;) {
    if(consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  //fprintf(stderr, "relational\n");
  Node *node = add();

  for(;;) {
    if(consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if(consume("<=")) {
      node = new_node(ND_LE, node, add());
    } else if(consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else if(consume(">=")) {
      node = new_node(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();

  for(;;) {
    if(consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if(consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *unary() {
  if(consume("+")) {
    return primary();
  }

  if(consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }

  return primary();
}

Node *primary() {
  //fprintf(stderr, "primary\n");
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);

    if (lvar) {
      //既存の変数なので、スタックのoffsetをノードに保存する
      node->offset = lvar->offset;
    } else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if(locals) {
        //fprintf(stderr,"%s, %d\n", tok->str,locals->offset);
        lvar->offset = locals->offset + 8;
      } else {
        lvar->offset = 8;
      }
      node->offset = lvar->offset;
    }

    locals = lvar;

    return node;
  }

  return new_node_num(expect_number());
}

bool at_eof() {
  return token->kind == TK_EOF;
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }

  return NULL;
}

