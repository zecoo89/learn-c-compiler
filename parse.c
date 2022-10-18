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
Node *fn_head;
Node *fn_tail;
Node *fn_def_head;
Node *fn_def_tail;

Node *new_node();
Node *new_node_num();

bool at_eof();

void program();
Node *fn_def();
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

// alternative of strndup
char * dup(char *str, int len) {
  char *buf = malloc(len + 1);
  memcpy(buf, str, len);
  buf[len] = '\0';

  return buf;
}

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
  //fprintf(stderr, "op: %s\n", op);
  //fprintf(stderr, "token->len: %d\n", token->len);
  //fprintf(stderr, "token->kind: %d\n", token->kind);
  //fprintf(stderr, "token->str: %s\n", token->str);
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

Token *consume_type(char *op) {
  Token *tok = consume_ident();
  if(!tok && memcmp(tok->str, op, token->len))
    return NULL;

  return tok;
}

void expect_type(char *op) {
  Token *tok = consume_ident();
  if(!tok || memcmp(tok->str, op, token->len))
    error("expect: '%s', actual: '%s'", "int", dup(tok->str, tok->len));

}

void expect(char *op) {
  //fprintf(stderr, "token->kind = %d\n", token->kind);
  //fprintf(stderr, "token->len = %d\n", token->len);
  //fprintf(stderr, "token->str = '%s'\n", token->str);
  //fprintf(stderr, "strlen(op) = %ld\n", strlen(op));
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    error("expect: '%s', actual: '%s'", op, dup(token->str, token->len));
  }

  token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM) {
    fprintf(stderr, "token->kind = %d\n", token->kind);
    fprintf(stderr, "token->len = %d\n", token->len);
    fprintf(stderr, "token->str = '%s'\n", token->str);
    error_at(dup(token->str, token->len), "数ではありません");
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
    code[i++] = fn_def();
  }

  code[i] = NULL;
}

Node *fn_def() {
  Node *node;

  expect_type("int");

  Token *tok = consume_ident();
  if (tok) {
    node = new_node(ND_DEF, NULL, NULL);
    node->name = dup(tok->str, tok->len);

    if(fn_head) {
      fn_tail->next_fn = node;
      fn_tail = node;
    } else {
      fn_head = fn_tail = node;
    }

    expect("(");
    if (!consume(")")) {
      int arg_num = 0;
      do {
        expect_type("int");
        tok = consume_ident();
        if (tok) {
          Node *nod = new_node(ND_LVAR, NULL, NULL);
          node->args[arg_num++] = nod;
          node->args_len = arg_num;

          LVar *lvar = find_lvar(tok);

          if (lvar) {
            //既存の変数なので、スタックのoffsetをノードに保存する
            nod->offset = lvar->offset;
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
            nod->offset = lvar->offset;
            locals = lvar;
          }
        } else {
          error_at(dup(token->str, token->len), "TK_IDENT以外のトークンです");
        }
      } while(!consume(")") && consume(","));
    }
    expect("{");

    Node *current = node;
    while(!consume("}")) {
      current->stmt = stmt();
      current = current->stmt;
    }

    return node;
  } else {
    error_at(dup(token->str, token->len), "関数定義ではありません");
  }

  //到達しないので意味はないが、コンパイラのwarningが出ないようにする
  return node;
}

Node *stmt() {
  Node *node;

  if(token->kind == TK_RETURN) {
    token = token->next;
    node = new_node(ND_RETURN, expr(), NULL);
  } else if(token->kind == TK_IF) {
    token = token->next;
    node = new_node(ND_IF, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    //fprintf(stderr, "[test] %s\n",token->str);
    if(token->kind == TK_ELSE) {
      token = token->next;
      node->els = stmt();
    }
    return node;
  } else if(token->kind == TK_WHILE) {
    token = token->next;
    node = new_node(ND_WHILE, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->body = stmt();
    return node;
  } else if(token->kind == TK_FOR) {
    token = token->next;
    node = new_node(ND_FOR, NULL, NULL);
    expect("(");
    node->init = expr();
    expect(";");
    node->cond = expr();
    expect(";");
    node->inc = expr();
    expect(")");
    node->body = stmt();
    return node;
  } else if(consume("{")) {
    node = new_node(ND_BLOCK, NULL, NULL);

    Node *current = node;
    while(!consume("}")) {
      current->stmt = stmt();
      current = current->stmt;
    }

    return node;
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

  if(consume("*")) {
    return unary();
  }

  if(consume("&")) {
    return unary();
  }

  return primary();
}


Node* function_call(Token *t) {
  Node *node = new_node(ND_CALL, NULL, NULL);
  node->name = dup(t->str, t->len);

  if(consume(")")) {
    return node;
  }

  int arg_num = 0;

  do {
    node->args[arg_num++] = expr();
  } while(consume(","));
  node->args_len = arg_num;
  expect(")");

  return node;
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
    // function call
    if(consume("(")) {
      return function_call(tok);
    }
    Node *node = new_node(ND_LVAR, NULL, NULL);
    node->name = dup(tok->str, tok->len);

    LVar *lvar = find_lvar(tok);

    if (lvar) {
      //fprintf(stderr, "name:%s, offset:%d\n", dup(lvar->name, lvar->len), lvar->offset);
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
        //fprintf(stderr, "[locals] name:%s, offset:%d\n", dup(lvar->name, lvar->len), lvar->offset);
      } else {
        lvar->offset = 8;
        //fprintf(stderr, "[ null ] name:%s, offset:%d\n", dup(lvar->name, lvar->len), lvar->offset);
      }
      node->offset = lvar->offset;
      locals = lvar;
    }


    return node;
  }

  return new_node_num(expect_number());
}

bool at_eof() {
  return token->kind == TK_EOF;
}

/* find left variable */
LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }

  return NULL;
}

