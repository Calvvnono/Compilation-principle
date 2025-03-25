#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 定义Token结构体
typedef struct {
    int type;
    char value[256];
} Token;

// 关键字表
char *keywords[] = {"begin", "if", "then", "while", "do", "end"};

// 获取下一个Token
Token getToken(FILE *fp) {
    Token token;
    char ch;
    char buffer[256];
    int i = 0;

    // 1. 跳过空白字符
    while ((ch = fgetc(fp)) != EOF && isspace(ch));

    if (ch == EOF) {
        token.type = -1; // 文件结束
        strcpy(token.value, "");
        return token;
    }
    // 2. 识别不同类型的Token
    if (isalpha(ch)) { // 标识符或关键字
        buffer[i++] = ch;
        while ((ch = fgetc(fp)) != EOF && (isalnum(ch))) {
            buffer[i++] = ch;
        }
        buffer[i] = '\0';
        ungetc(ch, fp); // 把多读的字符放回输入流

        // 检查是否是关键字
        int isKeyword = 0;
        for (int j = 0; j < 6; j++) {
            if (strcmp(buffer, keywords[j]) == 0) {
                token.type = j + 1; // 关键字的种别码
                isKeyword = 1;
                break;
            }
        }
        if (!isKeyword) {
            token.type = 10; // 标识符的种别码
        }
        strcpy(token.value, buffer);
    }
      else if (isdigit(ch)) { // 数字
        buffer[i++] = ch;
        while ((ch = fgetc(fp)) != EOF && isdigit(ch)) {
            buffer[i++] = ch;
        }
        buffer[i] = '\0';
        ungetc(ch, fp);
        token.type = 11; // 数字的种别码
        strcpy(token.value, buffer);
    }
       else { // 运算符和界符
        switch (ch) {
            case ':':
                ch = fgetc(fp);
                if(ch == '='){
                    token.type = 18;
                    strcpy(token.value, ":=");
                }
                else{
                    token.type = 17;
                    strcpy(token.value, ":");
                    ungetc(ch, fp);
                }

                break;
            case '+': token.type = 13; strcpy(token.value, "+"); break;
            case '-': token.type = 14; strcpy(token.value, "-"); break;
            case '*': token.type = 15; strcpy(token.value, "*"); break;
            case '/': token.type = 16; strcpy(token.value, "/"); break;
            case '<':
                ch = fgetc(fp);
                if(ch == '>'){
                    token.type = 21;
                    strcpy(token.value, "<>");
                }
                else if(ch == '='){
                     token.type = 22;
                     strcpy(token.value,"<=");
                }
                else{
                    token.type = 20;
                    strcpy(token.value, "<");
                     ungetc(ch, fp);
                }
                break;
            case '>':
              ch = fgetc(fp);
              if(ch == '='){
                  token.type = 24;
                  strcpy(token.value, ">=");
              }
              else{
                token.type = 23;
                strcpy(token.value, ">");
                ungetc(ch, fp);
              }
              break;
            case '=': token.type = 25; strcpy(token.value, "="); break;
            case ';': token.type = 26; strcpy(token.value, ";"); break;
            case '(': token.type = 27; strcpy(token.value, "("); break;
            case ')': token.type = 28; strcpy(token.value, ")"); break;
            case '#': token.type = 0; strcpy(token.value, "#"); break;
            default: token.type = -2; strcpy(token.value, "ERROR");break; // 无法识别的字符
        }

    }

    return token;
}

int main() {
    Token token;
    while ((token = getToken(stdin)).type != -1) { // 直到 EOF
       if(token.type != -2)
         printf("(%d,%s)\n", token.type, token.value);
    }
    return 0;
}