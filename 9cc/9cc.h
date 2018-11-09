#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_EOF,
};

enum {
    ND_NUM = 256,
    ND_IDENT,
    ND_EPSILON,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
} Node;

Token tokens[100];
Node *nodes[100];

int pos;
int node_count;
void tokenize(char *p);
void program();
void gen(Node *node);