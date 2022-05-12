%{
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>
    #include "sapphire.yy.h"
    size_t line_num = 1;
    extern int yylex();
%}

DIGIT [0-9]
CHAR [a-zA-Z]
CHAR_UND ({CHAR}|"_")

ID {CHAR_UND}({CHAR_UND}|{DIGIT})*
NUMBER {DIGIT}+

%option noyywrap
%option nomain

%x COMMENTS

%%

    /* keywords */
"FUNC" { return FUNC; }
"PARAM" { return PARAM; }
"VAR" { return VAR; }
"CALL" { return CALL; }
"RETURN" { return RETURN; }
"IF" { return IF; }
"WHILE" { return WHILE; }
"PRINT" { return PRINT; }

    /* operators */
"+" { return PLUS; } 
"-" { return MINUS; }
"=" { return EQUALS; } 
"==" { return DEQUALS; }

    /* symbols */
";" { return SEMICOLON; }
"(" { return LPAREN; }
")" { return RPAREN; }
[\{] { return LCURLY; }
[\}] { return RCURLY; }
"," { return COMMA; }

    /* primaries */
{ID} {   
    yylval.lexeme = strdup(yytext);
    return ID; 
}
{NUMBER} {   
    yylval.int_val = atoi(yytext);
    return NUMBER; 
}

    /* whitespace */
"//".* {}
[\/][*] { BEGIN(COMMENTS); }
<COMMENTS>[*][\/] { BEGIN(INITIAL);}
<COMMENTS>. {}
    /* no matter what count line numbers*/
<INITIAL,COMMENTS>\n { line_num++; }

[ \t] { }
. { return ERROR; }

%%
