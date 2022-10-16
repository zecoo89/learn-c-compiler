#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef MAC_FLAG
#define MOVZXXX "movzx"
#else
#define MOVZXXX "movzb"
#endif

extern Node *fn_head;
extern Node *fn_tail;

char *registers[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_prologue() {
  // アセンブリの最初の部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl");

  Node *cur = fn_head;
  if(cur) {
#ifdef MAC_FLAG
    printf(" _%s", cur->name);
#else
    printf(" %s", cur->name);
#endif
    cur = cur->next_fn;
  }
  while(cur) {
#ifdef MAC_FLAG
    printf(", _%s", cur->name);
#else
    printf(", %s", cur->name);
#endif
    cur = cur->next_fn;
  }
  printf("\n");


}

void gen_fn_epilogue() {
  printf("  pop rax\n");
  printf("# epilogue\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

void gen_codes(Node *code[]) {
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
  }
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }

  printf("# var %s\n", node->name);
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
    case ND_DEF:
#ifdef MAC_FLAG
      printf("_%s:\n", node->name); // for Mac support
#else
      printf("%s:\n", node->name);
#endif

      printf("# prologue\n");
      printf("  push rbp\n");
      printf("  mov rbp, rsp\n");
      printf("  sub rsp, 208\n");

      for(int i=0;i<node->args_len;i++) {
        printf("  mov [rbp-%d], %s\n", 8*(i+1),registers[i]);
      }

      cur = node->stmt;
      while(cur) {
        gen(cur);
        //1つ1つのステートメントは1つの値をスタックに残すので、
        //その値を毎回ポップさせる。
        //printf("  pop rax\n");
        cur = cur->stmt;
      }

      gen_fn_epilogue();
      return;

    case ND_CALL:
      for(int i=node->args_len-1;i>=0;i--) {
        //TODO 現状だと数値以外だと0を引数に渡すことになるので、
        //変数や演算に対応する。
        gen(node->args[i]);
        printf("  pop rax\n");
        printf("  mov %s, rax\n", registers[i]);
        //printf("  mov %s, %d\n", registers[i], node->args[i]->val);
      }
#ifdef MAC_FLAG
      printf("  call _%s\n", node->name);
#else
      printf("  call %s\n", node->name);
#endif
      // 呼び出した関数内部の最後のpop raxとcall後のpop raxが重複してしまうので、
      // call後にpush raxを入れることで関数内部の最後のpop raxを打ち消す
      printf("  push rax\n");
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
        //printf("  pop rax\n");
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
