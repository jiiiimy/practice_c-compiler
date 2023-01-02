#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Types of token
typedef enum
{
    TK_RESERVED, // Sign
    TK_NUM,      // Number
    TK_EOF,      // End of file
} TokenKind;

typedef struct Token Token;
// Token Type
struct Token
{
    TokenKind kind; // Type of token
    Token *next;    // Next token
    int val;        // Number, when kind is TK_NUM
    char *str;      // String of token
};
// Current feature token
Token *token;
// input
char *user_input;

void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // output pos count blanks
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op)
{
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

void expect(char op)
{
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "'%c'ではありません", op);
    token = token->next;
}

int expect_number()
{
    if (token->kind != TK_NUM)
        error_at(token->str, "数値ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-')
        {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数が正しくありません\n");
        return 1;
    }

    token = tokenize(argv[1]);

    char *p = argv[1];

    printf(".globl main\n");
    printf("main:\n");
    printf("  mov x0, %ld\n", expect_number());

    while (!at_eof())
    {
        if (consume('+'))
        {
            printf("  add x0, x0, %d\n", expect_number());
        }

        if (consume('-'))
        {
            printf("  sub x0, x0, %d\n", expect_number());
        }

        printf("  ret\n");
        return 0;
    }

    printf("  ret\n");
    return 0;
}