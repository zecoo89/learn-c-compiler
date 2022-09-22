typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

// 抽象構文木のノード種類
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_ASSIGN, // =
  ND_LVAR, // local variable
  ND_NUM, // integer
  ND_EQ, // ==
  ND_NE, // !=
  ND_LT, // <=
  ND_LE, // <
  ND_RETURN, // return
  ND_IF, // if,
  ND_ELSE, // else,
  ND_WHILE, // while
  ND_FOR, // for
  ND_BLOCK, //{  }
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;
  Node * lhs;
  Node * rhs;
  int val;
  int offset;

  //if(cond) then else els;
  // while(cond) body
  // for(init;cond;inc) body
  Node *cond;
  Node *then;
  Node *els;
  Node *body;
  Node *init;
  Node *inc;

  // to represent blocks '{stmt*}'
  Node *stmt;

  // to generate node tree image
  char var_name[20];
};

typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

void parse();
Token *tokenize(char *);
void gen(Node *);

void error(char *, ...);

