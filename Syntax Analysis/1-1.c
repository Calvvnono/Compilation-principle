#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 10000

char input[MAX_LEN]; // 输入程序内容
int pos = 0;

// 跳过空格与换行
void skip_whitespace() {
    while (isspace(input[pos])) pos++;
}

// 读取标识符（ID）
void read_word(char *word) {
    int i = 0;
    while (isalnum(input[pos]) || input[pos] == '_') {
        word[i++] = input[pos++];
    }
    word[i] = '\0';
}

// 打印缩进
void indent_print(int indent, const char *format, const char *content) {
    printf("%*s", indent, "");
    printf(format, content);
    printf("\n");
}

// 解析表达式声明（递归向下）
void expression(int indent);
void factor(int indent) {
    skip_whitespace();
    if (isdigit(input[pos])) {
        char num[100];
        int i = 0;
        while (isdigit(input[pos])) num[i++] = input[pos++];
        num[i] = '\0';
        indent_print(indent, "因子: %s", num);
    } else if (isalpha(input[pos])) {
        char id[100];
        read_word(id);
        indent_print(indent, "因子: %s", id);
    } else if (input[pos] == '(') {
        indent_print(indent, "因子: (", "");
        pos++;
        expression(indent + 4);
        skip_whitespace();
        if (input[pos] == ')') {
            indent_print(indent, ")", "");
            pos++;
        } else {
            printf("错误：缺少 )\n");
        }
    } else {
        printf("错误：无效的因子\n");
    }
}

void term(int indent) {
    indent_print(indent, "项:", "");
    factor(indent + 4);
    skip_whitespace();
    while (input[pos] == '*' || input[pos] == '/') {
        char op = input[pos++];
        char op_str[2] = {op, '\0'};
        indent_print(indent + 4, "操作符: %s", op_str);
        factor(indent + 4);
        skip_whitespace();
    }
}

void expression(int indent) {
    indent_print(indent, "表达式:", "");
    term(indent + 4);
    skip_whitespace();
    while (input[pos] == '+' || input[pos] == '-') {
        char op = input[pos++];
        char op_str[2] = {op, '\0'};
        indent_print(indent + 4, "操作符: %s", op_str);
        term(indent + 4);
        skip_whitespace();
    }
}

void logical_expression(int indent) {
    indent_print(indent, "逻辑运算:", "");
    expression(indent + 4);
    skip_whitespace();

    if (strchr("<>=", input[pos])) {
        char op[3] = {0};
        op[0] = input[pos++];
        if ((op[0] == '<' && (input[pos] == '=' || input[pos] == '>')) ||
            (op[0] == '>' && input[pos] == '=') ||
            (op[0] == '=')) {
            op[1] = input[pos++];
        }
        indent_print(indent + 4, "逻辑运算符: %s", op);
        expression(indent + 4);
    }
}

void assignment_statement(int indent) {
    indent_print(indent, "赋值语句:", "");
    char id[100];
    read_word(id);
    indent_print(indent + 4, "标识符: %s", id);
    skip_whitespace();

    if (input[pos] == ':' && input[pos + 1] == '=') {
        pos += 2;
        skip_whitespace();
        expression(indent + 4);
    } else {
        printf("错误：缺少 :=\n");
    }
}

void statement_list(int indent);

void if_statement(int indent) {
    indent_print(indent, "条件语句:", "");
    pos += 2; // 跳过 "if"
    skip_whitespace();
    logical_expression(indent + 4);
    skip_whitespace();
    if (strncmp(&input[pos], "then", 4) == 0) {
        pos += 4;
        skip_whitespace();
        statement_list(indent + 4);
        skip_whitespace();
        if (strncmp(&input[pos], "end", 3) == 0) {
            pos += 3;
            indent_print(indent, "结束条件语句", "");
        } else {
            printf("错误：缺少 end\n");
        }
    } else {
        printf("错误：缺少 then\n");
    }
}

void while_statement(int indent) {
    indent_print(indent, "循环语句:", "");
    pos += 5;
    skip_whitespace();
    logical_expression(indent + 4);
    skip_whitespace();
    if (strncmp(&input[pos], "do", 2) == 0) {
        pos += 2;
        skip_whitespace();
        statement_list(indent + 4);
        skip_whitespace();
        if (strncmp(&input[pos], "end", 3) == 0) {
            pos += 3;
            indent_print(indent, "结束循环语句", "");
        } else {
            printf("错误：缺少 end\n");
        }
    } else {
        printf("错误：缺少 do\n");
    }
}

void statement(int indent) {
    skip_whitespace();
    if (isalpha(input[pos])) {
        // 判断关键字
        if (strncmp(&input[pos], "if", 2) == 0) {
            if_statement(indent);
        } else if (strncmp(&input[pos], "while", 5) == 0) {
            while_statement(indent);
        } else {
            assignment_statement(indent);
        }
    } else {
        printf("错误：无效语句\n");
    }
    skip_whitespace();
    if (input[pos] == ';') {
        pos++;
    }
}

void statement_list(int indent) {
    indent_print(indent, "语句串:", "");
    while (input[pos] != '\0' && strncmp(&input[pos], "end", 3) != 0) {
        statement(indent + 4);
        skip_whitespace();
    }
}

void program() {
    int indent = 0;
    printf("程序:\n");
    if (strncmp(&input[pos], "begin", 5) == 0) {
        pos += 5;
        skip_whitespace();
        statement_list(indent + 4);
        skip_whitespace();
        if (strncmp(&input[pos], "end", 3) == 0) {
            pos += 3;
            printf("结束\n");
        } else {
            printf("错误：缺少 end");
        }
    } else {
        printf("错误1-1：缺少 begin");
    }
}


int main() {
    // 读取多行输入
    char line[512];
    while (fgets(line, sizeof(line), stdin)) {
        strcat(input, line);
    }
    program();
    return 0;
}