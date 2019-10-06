%{
int cur_line_num = 1;
void init_scanner();
void lex_error(char* msg, int line);
void output(char* id, char* info, int line);
%}

/* Definitions, note: \042 is '"' */
INTEGER             ([0-9]+)
UNTERM_STRING       (\042[^\042\n]*)
STRING              (\042[^\042\n]*\042)
IDENTIFIER          ([_a-zA-Z][_a-zA-Z0-9]*)
OPERATOR            ([\*\-\+\<\>\=\!\%\/\^])
DELIMITER           ([,;:\.\(\)\{}\[\]\?])
SINGLE_COMMENT1     ("//"[^\n]*)
SINGLE_COMMENT2     ("#"[^\n]*)
SINGLE_COMMENT3     ([/][*][^*]*[*]+([^*/][^*]*[*]+)*[/])
SINGLE_COMMENT4     ("@"[^\n]*)
FLOAT               (^[1-9]\d*\.\d*|0\.\d*[1-9]\d*$)

%%

[\n]                { cur_line_num++;                       }
[ \t\r\a]+          { /* ignore all spaces */               }
{SINGLE_COMMENT1}   { /* skip for single line comment */    }
{SINGLE_COMMENT2}   { /* skip for single line commnet */    }
{SINGLE_COMMENT3}   { /*skip */                                      }
{SINGLE_COMMENT4}   { return 9;         }
{OPERATOR}          { return 1;         }

"<="                { return 2;              }
">="                { return 2;              }
"=="                { return 2;              }
"!="                { return 2;              }
"&&"                { return 2;              }
"||"                { return 2;              }
"void"              { return 3;            }
"int"               { return 3;            }
"package"           { return 3;            }
"if"                { return 3;            }
"else"              { return 3;            }
"return"            { return 3;            }
"break"             { return 3;            }
"continue"          { return 3;            }
"public"            { return 3;            }
"private"           { return 3;            }
"final"             { return 3;            }
"String"            { return 3;            }
"import"			{ return 3;			   }
{INTEGER}           { return 4;     }
{STRING}            { return 5;  }
{IDENTIFIER}        { return 6;      }
{FLOAT}             { return 7; }
{DELIMITER}         { return 8; }
<<EOF>>             { return 0; }

.                   { lex_error("Unrecognized character", cur_line_num);        }

%%

int main(int argc, char* argv[]) {
    int token;
    init_scanner();
    while (token = yylex()) {
        if (token == 1) output("operator", yytext, cur_line_num);
        if (token == 2) output("binary operator",yytext, cur_line_num);
        if (token == 3) output("keyword", yytext, cur_line_num);
        if (token == 4) output("integer", yytext, cur_line_num);
        if (token == 5) output("string", yytext, cur_line_num);
        if (token == 6) output("identifier", yytext, cur_line_num);
        if (token == 7) output("float", yytext, cur_line_num);
        if (token == 8) output("delimiter", yytext, cur_line_num);
        if (token == 9) output("note", yytext, cur_line_num);
    }
    return 0;
}

void output(char* id, char* info, int line){
    printf("<%s, %s, lines:%d >\n", id, info, line);
}

void init_scanner() {
    printf("%-20s%s\n", "TOKEN-TYPE", "TOKEN-VALUE");
    printf("-------------------------------------------------\n");
}

void lex_error(char* msg, int line) {
    printf("\nError at line %-3d: %s\n\n", line, msg);
}

int yywrap(void) {
    return 1;
}
