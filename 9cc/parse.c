#include "9cc.h"

void program2();
Node *assign();
Node *assign2();
Node *term();
Node *mul();
Node *expr();

void tokenize(char *p) {
    int i = 0;
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';') {
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

        if ('a' <= *p && *p <= 'z') {
            tokens[i].ty = TK_IDENT;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

void error(int i) {
    fprintf(stderr, "予期せぬトークンです: %s\n", tokens[i].input);
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

Node *new_node_ident (char *name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = *name;
    return node;
}

Node *new_node_epsilon() {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_EPSILON;
    return node;
}

void program() {
    nodes[node_count++] = assign();
    program2();
}

void program2() {
    if (tokens[pos].ty != TK_EOF) {
        nodes[node_count++] = assign();
        program2();
    }
}

Node *assign() {
    Node *lhs = expr();
    Node *rhs = assign2();
    if (tokens[pos].ty != ';') {
        error(pos);
        return 0;
    } else {
        pos++;
        if (rhs->ty == ND_EPSILON) {
            return lhs;
        } else {
            return new_node('=', lhs, rhs);
        }
    }
}

Node *assign2() {
    if (tokens[pos].ty != ';') {
        if (tokens[pos].ty != '=') {
            error(pos);
        }
        pos++;
        Node *lhs = expr();
        Node *rhs = assign2();
        if (rhs->ty == ND_EPSILON) {
            return lhs;
        } else {
            return new_node('=', lhs, rhs);
        }
    } else {
        return new_node_epsilon();
    }
}

Node *term() {
    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    } else if (tokens[pos].ty == TK_IDENT) {
        return new_node_ident(tokens[pos++].input);
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
    } else {
        return lhs;
    }
}

Node *expr() {
    Node *lhs = mul();
    if (tokens[pos].ty == TK_EOF) {
        return lhs;
    } else if (tokens[pos].ty == '+' || tokens[pos].ty == '-') {
        return new_node(tokens[pos++].ty, lhs, expr());
    } else {
        return lhs;
    }
}