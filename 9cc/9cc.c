#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型を表す値
enum {
    TK_NUM = 256, // 整数トークン
    TK_EOF,       // 入力の終わりを表すトークン
};

enum {
    ND_NUM = 256,
};

// トークンの型
typedef struct {
    int ty;      // トークンの型
    int val;     // tyがTK_NUMの場合、その数値
    char *input; // トークン文字列（エラーメッセージ用）
} Token;

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

Node *term();
Node *mul();
Node *expr();

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
Token tokens[100];
int pos;

// pが指している文字列をトークンに分割してtokensに保存する
void tokenize(char *p) {
    // printf("呼んだ？\n");
    int i = 0;
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

// エラーを報告するための関数
void error(int i) {
    fprintf(stderr, "予期せぬトークンです: %s\n",
            tokens[i].input);
    exit(1);
}

Node *new_node (int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num (int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

Node *term() {
    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    } else if (tokens[pos].ty == '(') {
        pos++;
        Node *node = expr();
        if (tokens[pos].ty != ')') {
            error(pos);
        }
        pos++;
        return node;
    } else {
        error(pos);
        return 0;
    }
}

Node *mul() {
    Node *lhs = term();
    if (tokens[pos].ty == TK_EOF) {
        return lhs;
    } else if (tokens[pos].ty == '*' || tokens[pos].ty == '/') {
        return new_node(tokens[pos++].ty, lhs, mul());
    } else if (tokens[pos].ty == '+' || tokens[pos].ty == '-' || tokens[pos].ty == '(' || tokens[pos].ty == ')') {
        return lhs;
    } else {
        error(pos);
        return 0;
    }
}

Node *expr() {
    Node *lhs = mul();
    if (tokens[pos].ty == TK_EOF) {
        return lhs;
    } else if (tokens[pos].ty == '+' || tokens[pos].ty == '-') {
        return new_node(tokens[pos++].ty, lhs, expr());
    } else if (tokens[pos].ty == '*' || tokens[pos].ty == '/' || tokens[pos].ty == '(' || tokens[pos].ty == ')') {
        return lhs;
    } else {
        error(pos);
        return 0;
    }
}

void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    } else {
        gen(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");

        switch (node->ty) {
            case '+':
                printf("  add rax, rdi\n");
                break;
            case '-':
                printf("  sub rax, rdi\n");
                break;
            case '*':
                printf("  mul rdi\n");
                break;
            case '/':
                printf("  mov rdx, 0\n");
                printf("  div rdi\n");
        }

        printf("  push rax\n");
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // トークナイズする
    tokenize(argv[1]);

    pos = 0;
    Node *node = expr();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");

    return 0;
}