#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_QUADS 500
#define MAX_TOKEN_LEN 64
#define MAX_TOKENS 1000
#define MAX_INPUT 10000

// 四元式结构
typedef struct {
    char op[16];
    char arg1[16];
    char arg2[16];
    char result[16];
} Quad;

static Quad quads[MAX_QUADS];
static int quadCount = 0;
static int tempCount = 0;
static int labelCount = 0;

static char* tokens[MAX_TOKENS];
static int tokenCount = 0;
static int curToken = 0;

// 生成新的临时变量
char* newTemp() {
    char *t = (char*)malloc(16);
    sprintf(t, "t%d", ++tempCount);
    return t;
}

// 生成新的标签
char* newLabel() {
    char *l = (char*)malloc(16);
    sprintf(l, "L%d", ++labelCount);
    return l;
}

// 添加四元式
void addQuad(const char* op, const char* a1, const char* a2, const char* res) {
    strcpy(quads[quadCount].op, op);
    strcpy(quads[quadCount].arg1, a1);
    strcpy(quads[quadCount].arg2, a2);
    strcpy(quads[quadCount].result, res);
    quadCount++;
}

// 打印四元式
void printQuads() {
    for (int i = 0; i < quadCount; i++) {
        Quad *q = &quads[i];
        if (strcmp(q->op, "label") == 0) {
            printf("%s:\n", q->result);
        } else if (strcmp(q->op, "ifFalse") == 0) {
            printf("ifFalse %s goto %s\n", q->arg1, q->result);
        } else if (strcmp(q->op, "goto") == 0) {
            printf("goto %s\n", q->result);
        } else if (strcmp(q->op, ":=") == 0) {
            printf("%s = %s\n", q->result, q->arg1);
        } else if (strcmp(q->op, "<") == 0 || strcmp(q->op, ">") == 0) {
            // 在 < 和 > 后添加额外空格
            printf("%s = %s %s  %s\n", q->result, q->arg1, q->op, q->arg2);
        } else {
            printf("%s = %s %s %s\n", q->result, q->arg1, q->op, q->arg2);
        }
    }
}

// 词法分析
void tokenize(const char* input) {
    int i = 0;
    char buf[MAX_TOKEN_LEN];
    while (input[i]) {
        if (isspace(input[i])) { i++; continue; }
        // 两字符运算符
        if ((input[i] == ':' && input[i+1] == '=') ||
            (input[i] == '<' && input[i+1] == '=') ||
            (input[i] == '>' && input[i+1] == '=') ||
            (input[i] == '=' && input[i+1] == '=') ||
            (input[i] == '!' && input[i+1] == '=')) {
            buf[0] = input[i]; buf[1] = input[i+1]; buf[2] = '\0';
            tokens[tokenCount++] = strdup(buf);
            i += 2;
        }
        else if (isalpha(input[i])) {
            int j = 0;
            while (isalnum(input[i])) buf[j++] = input[i++];
            buf[j] = '\0'; tokens[tokenCount++] = strdup(buf);
        }
        else if (isdigit(input[i])) {
            int j = 0;
            while (isdigit(input[i])) buf[j++] = input[i++];
            buf[j] = '\0'; tokens[tokenCount++] = strdup(buf);
        }
        else {
            buf[0] = input[i++]; buf[1] = '\0';
            tokens[tokenCount++] = strdup(buf);
        }
        if (tokenCount >= MAX_TOKENS-1) break;
    }
    tokens[tokenCount] = NULL;
}

char* peekToken() {
    return (curToken < tokenCount) ? tokens[curToken] : NULL;
}

char* getToken() {
    return (curToken < tokenCount) ? tokens[curToken++] : NULL;
}

void expect(const char* sym) {
    char* t = peekToken();
    if (!t || strcmp(t, sym) != 0) {
        fprintf(stderr, "Error: expected '%s' but got '%s'\n", sym, t ? t : "NULL");
        exit(EXIT_FAILURE);
    }
    getToken();
}

// 语法分析函数声明
void parseProgram();
void parseBlock();
void parseStmtList();
void parseStmt();
void parseAssign();
char* parseCondition();
char* parseExpr();
char* parseTerm();
char* parseFactor();

int main() {
    // 读取 stdin
    static char input[MAX_INPUT];
    int len = 0, c;
    while ((c = getchar()) != EOF && len < MAX_INPUT - 1) {
        input[len++] = (char)c;
    }
    input[len] = '\0';

    tokenize(input);
    parseProgram();
    printQuads();
    return 0;
}

void parseProgram() {
    expect("main"); expect("("); expect(")");
    parseBlock();
}

void parseBlock() {
    expect("{");
    parseStmtList();
    expect("}");
}

void parseStmtList() {
    while (peekToken() && strcmp(peekToken(), "}") != 0) {
        parseStmt();
        if (peekToken() && strcmp(peekToken(), ";") == 0) getToken();
    }
}

void parseStmt() {
    char* t = peekToken();
    if (!t) return;
    if (strcmp(t, "if") == 0) {
        getToken();
        char* condTemp = parseCondition();
        char* l_false = newLabel();
        addQuad("ifFalse", condTemp, "", l_false);
        parseBlock();
        addQuad("label", "", "", l_false);
    }
    else if (strcmp(t, "while") == 0) {
        getToken();
        char* l_begin = newLabel();
        addQuad("label", "", "", l_begin);
        char* condTemp = parseCondition();
        char* l_end = newLabel();
        addQuad("ifFalse", condTemp, "", l_end);
        parseBlock();
        addQuad("goto", "", "", l_begin);
        addQuad("label", "", "", l_end);
    }
    else {
        parseAssign();
    }
}

void parseAssign() {
    char* id = getToken();
    expect(":=");
    char* exprTemp = parseExpr();
    addQuad(":=", exprTemp, "", id);
}

char* parseCondition() {
    char* left = parseExpr();
    char* op = getToken();
    char* right = parseExpr();
    char* t = newTemp();
    addQuad(op, left, right, t);
    return t;
}

char* parseExpr() {
    char* t = parseTerm();
    while (peekToken() && (strcmp(peekToken(), "+") == 0 || strcmp(peekToken(), "-") == 0)) {
        char* op = getToken();
        char* t2 = parseTerm();
        char* r = newTemp();
        addQuad(op, t, t2, r);
        t = r;
    }
    return t;
}

char* parseTerm() {
    char* t = parseFactor();
    while (peekToken() && (strcmp(peekToken(), "*") == 0 || strcmp(peekToken(), "/") == 0)) {
        char* op = getToken();
        char* f2 = parseFactor();
        char* r = newTemp();
        addQuad(op, t, f2, r);
        t = r;
    }
    return t;
}

char* parseFactor() {
    char* t = peekToken();
    if (!t) { fprintf(stderr, "Unexpected end of input in factor\n"); exit(EXIT_FAILURE); }
    if (isdigit(t[0])) {
        return getToken();
    }
    else if (isalpha(t[0])) {
        return getToken();
    }
    else if (strcmp(t, "(") == 0) {
        getToken();
        char* e = parseExpr();
        expect(")");
        return e;
    }
    else {
        fprintf(stderr, "Unexpected token '%s' in factor\n", t);
        exit(EXIT_FAILURE);
    }
    return NULL;
}