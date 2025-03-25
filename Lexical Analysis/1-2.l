int num_lines=1,num_chars=0;
%%
\n ++num_lines;
.  ++num_chars;
%%
int main()
{
    yylex();
    printf("Lines=%d,Chars=%d",num_lines,num_chars);
}