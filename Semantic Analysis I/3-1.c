#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 4096

char input[MAX_SIZE] = "";
int pos = 0;
int temp_count = 1; // 临时变量计数器从1开始

// 四元式结构体
typedef struct {
    char *op;
    char *arg1;
    char *arg2;
    char *result;
} Quad;

Quad *quads = NULL;
int quad_count = 0;
int quad_capacity = 0;

// 添加四元式到数组
void add_quad(const char *op, const char *arg1, const char *arg2, const char *result) {
    if (quad_count >= quad_capacity) {
        int new_cap = quad_capacity == 0 ? 4 : quad_capacity * 2;
        Quad *new_quads = realloc(quads, new_cap * sizeof(Quad));
        if (!new_quads) {
            perror("内存分配失败");
            exit(EXIT_FAILURE);
        }
        quads = new_quads;
        quad_capacity = new_cap;
    }
    Quad *q = &quads[quad_count];
    q->op = op ? strdup(op) : NULL;
    q->arg1 = arg1 ? strdup(arg1) : NULL;
    q->arg2 = arg2 ? strdup(arg2) : NULL;
    q->result = result ? strdup(result) : NULL;
    quad_count++;
}

// 生成临时变量名
char* new_temp() {
    static char temp[10];
    sprintf(temp, "t%d", temp_count++);
    return temp;
}

// 跳过空白字符
void skip_whitespace() {
    while (input[pos] == ' ' || input[pos] == '\t' || input[pos] == '\n' || input[pos] == '\r') {
        pos++;
    }
}

// 读取标识符
void read_word(char *buffer) {
    int i = 0;
    while (isalnum(input[pos])) {
        buffer[i++] = input[pos++];
    }
    buffer[i] = '\0';
}

// 读取符号（操作符）
void read_symbol(char *buffer) {
    buffer[0] = input[pos++];
    buffer[1] = '\0';
}

// 因子处理（数字、标识符、括号表达式）
char* factor();
// 项处理（乘除）
char* term();
// 表达式处理（加减）
char* expression();

char* factor() {
    skip_whitespace();
    if (isdigit(input[pos])) {
        char num[32];
        int i = 0;
        while (isdigit(input[pos])) {
            num[i++] = input[pos++];
        }
        num[i] = '\0';
        return strdup(num);
    } else if (isalpha(input[pos])) {
        char id[32];
        read_word(id);
        return strdup(id);
    } else if (input[pos] == '(') {
        pos++;
        char *res = expression();
        skip_whitespace();
        if (input[pos] == ')') {
            pos++;
            return res;
        } else {
            fprintf(stderr, "错误：缺少右括号\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "错误：无效的因子\n");
        exit(EXIT_FAILURE);
    }
}

char* term() {
    char *left = factor();
    skip_whitespace();
    while (input[pos] == '*' || input[pos] == '/') {
        char op[2];
        op[0] = input[pos++];
        op[1] = '\0';
        char *right = factor();
        char *temp = new_temp();
        add_quad(op, left, right, temp);
        free(left);
        free(right);
        left = strdup(temp);
        skip_whitespace();
    }
    return left;
}

char* expression() {
    char *left = term();
    skip_whitespace();
    while (input[pos] == '+' || input[pos] == '-') {
        char op[2];
        op[0] = input[pos++];
        op[1] = '\0';
        char *right = term();
        char *temp = new_temp();
        add_quad(op, left, right, temp);
        free(left);
        free(right);
        left = strdup(temp);
        skip_whitespace();
    }
    return left;
}

void assignment_statement() {
    char id[32];
    read_word(id);
    skip_whitespace();
    if (input[pos] == ':' && input[pos+1] == '=') {
        pos += 2;
        skip_whitespace();
        char *res = expression();
        add_quad("=", res, NULL, id);
        free(res);
    } else {
        fprintf(stderr, "错误：缺少赋值符号:=\n");
        exit(EXIT_FAILURE);
    }
}

void statement() {
    skip_whitespace();
    if (isalpha(input[pos])) {
        assignment_statement();
    } else {
        fprintf(stderr, "错误：无效的语句\n");
        exit(EXIT_FAILURE);
    }
    skip_whitespace();
    if (input[pos] == ';') {
        pos++;
    }
}

void statement_list() {
    while (input[pos] != '\0' && strncmp(input + pos, "end", 3) != 0) {
        statement();
        skip_whitespace();
    }
}

void program() {
    if (strncmp(input + pos, "begin", 5) == 0) {
        pos += 5;
        skip_whitespace();
        statement_list();
        if (strncmp(input + pos, "end", 3) == 0) {
            pos += 3;
        } else {
            fprintf(stderr, "错误：缺少end\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "错误：缺少begin\n");
        exit(EXIT_FAILURE);
    }
}

// 打印四元式
void print_quads() {
    for (int i = 0; i < quad_count; i++) {
        Quad q = quads[i];
        if (strcmp(q.op, "=") == 0) {
            printf("%s=%s\n", q.result, q.arg1);
        } else {
            printf("%s=%s%s%s\n", q.result, q.arg1, q.op, q.arg2);
        }
    }
}

// 释放四元式内存
void free_quads() {
    for (int i = 0; i < quad_count; i++) {
        Quad q = quads[i];
        free(q.op);
        free(q.arg1);
        free(q.arg2);
        free(q.result);
    }
    free(quads);
}

int main() {
    char buffer[MAX_SIZE];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (strlen(input) + strlen(buffer) < MAX_SIZE) {
            strcat(input, buffer);
        } else {
            fprintf(stderr, "输入超出最大长度限制\n");
            return EXIT_FAILURE;
        }
    }
    program();
    print_quads();
    free_quads();
    return 0;
}