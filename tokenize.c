#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

extern char *user_input;
extern Token *token;
int line_number = 0;
int from_head = 0;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;

  for(;user_input[pos] != '\n' && pos  > 0;pos--) {}
  pos++;

  int col = 0;
  for(;user_input[pos] != '\n';pos++) {
    fprintf(stderr, "%c", user_input[pos]);
    col++;
  }
  col --;

  fprintf(stderr, "\n");
  fprintf(stderr, "%*s", col, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

Token *new_token(TokenKind kind, Token *cur, char * str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;

  tok->line_number = line_number;
  tok->from_head = from_head;
  return tok;
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

bool is_new_line(char c) {
  if (c == '\n') {
    return 1;
  } else {
    return 0;
  }
}

Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (is_new_line(*p)) {
      p++;
      line_number++;
      from_head = 0;
      //fprintf(stderr, "new line! -> %d\n", line_number);
      continue;
    }

    if (isspace(*p)) {
      p++;
      from_head++;
      continue;
    }

    if(startswith(p, "==") ||
        startswith(p, "!=") ||
        startswith(p, "<=") ||
        startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      from_head += 2;
      continue;
    }

    if (*p == '=' || *p == ';' ||
        *p == '<' || *p == '>' ||
        *p == '(' || *p == ')' ||
        *p == '+' || *p == '-' ||
        *p == '*' || *p == '/' ||
        *p == '{' || *p == '}' ||
        *p == ',' || *p == '&'
        ) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      from_head++;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p, 0);
      char *q = p;

      while (('a' <= *p && *p <= 'z') || *p == '_') {
        p++;
        from_head++;
      }

      cur->len = p - q;
      from_head += p - q;

      //TODO ident()

      if(cur->len == 6 && memcmp("return", q, 6) == 0) {
        cur->kind = TK_RETURN;
      } else if(cur->len == 2 && memcmp("if", q, 2) == 0) {
        cur->kind = TK_IF;
      } else if(cur->len == 4 && memcmp("else", q, 4) == 0) {
        cur->kind = TK_ELSE;
      } else if(cur->len == 5 && memcmp("while", q, 5) == 0) {
        cur->kind = TK_WHILE;
      } else if(cur->len == 3 && memcmp("for", q, 3) == 0) {
        cur->kind = TK_FOR;
      }

      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      from_head += p - q;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);

  token = head.next;
  return head.next;
}

