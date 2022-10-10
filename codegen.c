#include "9cc.h"
#include <stdio.h>

#ifdef MAC_FLAG
#define MOVZXXX "movzx"
#else
#define MOVZXXX "movzb"
#endif

extern Node *func_head;
extern Node *func_tail;

void gen_prologue() {
  // アセンブリの最初の部分を出力
  printf(".intel_syntax noprefix\n");

#ifdef MAC_FLAG
  printf(".globl _main");
#else
  printf(".globl main");
#endif
  if (func_head) {
    while(func_head) {
      printf(", %s", func_head->name);
      func_head = func_head->next_func;
    }
  }
  printf("\n");

#ifdef MAC_FLAG
  printf("_main:\n"); // for Mac support
#else
  printf("main:\n");
#endif

  //プロローグ
  //変数26個分の領域を確保する
  printf("# prologue\n");
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");
}

void gen_epilogue() {
  printf("# epilogue\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

void gen_codes(Node *code[]) {
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    printf("# end_line\n");
    printf("  pop rax\n");
  }
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }

  printf("# push variable address\n");
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

int label_number = 0;

void gen(Node *node) {
  int local_label_number = label_number;
  Node *cur;

  switch (node->kind) {
    case ND_CALL:
#ifdef OS_FLAG
      printf("  call %s\n", node->name);
#else
      printf("  call _%s\n", node->name);
#endif
      return;
    case ND_IF:
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .Lelse%d\n", local_label_number);
      gen(node->then);
      printf("  jmp .Lend%d\n", local_label_number);
      printf(".Lelse%d:\n", local_label_number);
      if(node->els)
        gen(node->els);
      printf(".Lend%d:\n", local_label_number);
      label_number++;
      return;
    case ND_WHILE:
      printf(".Lbegin%d:\n", local_label_number);
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", local_label_number);
      gen(node->body);
      printf("  jmp .Lbegin%d\n", local_label_number);
      printf(".Lend%d:\n", local_label_number);
      label_number++;
      return;
    case ND_FOR:
      gen(node->init);
      printf(".Lbegin%d:\n", local_label_number);
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .Lend%d\n", local_label_number);
      gen(node->body);
      gen(node->inc);
      printf("  jmp .Lbegin%d\n", local_label_number);
      printf(".Lend%d:\n", local_label_number);
      label_number++;
      return;
    case ND_BLOCK:
      cur = node->stmt;
      while(cur) {
        gen(cur);
        //1つ1つのステートメントは1つの値をスタックに残すので、
        //その値を毎回ポップさせる。
        printf("  pop rax\n");
        cur = cur->stmt;
      }
      return;
    case ND_RETURN:
      gen(node->lhs);
      printf("# RETURN\n");
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_NUM:
      printf("# NUM\n");
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("# push variable data\n");
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("# ASSIGN\n");
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
    default:
      break;
  }


  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("# ADD\n");
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("# SUB\n");
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("# MUL\n");
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("# DIV\n");
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("# EQ\n");
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  %s rax, al\n", MOVZXXX);
      break;
    case ND_NE:
      printf("# NE\n");
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  %s rax, al\n", MOVZXXX);
      break;
    case ND_LT:
      printf("# ND_LT\n");
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  %s rax, al\n", MOVZXXX);
      break;
    case ND_LE:
      printf("# ND_LE\n");
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  %s rax, al\n", MOVZXXX);
      break;
    default:
      break;
  }

  printf("  push rax\n");
}
