
%{
    #include <stdio.h>
    #include <stdlib.h>
    // stuff from flex that bison needs to know about:
    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
    extern int line_num;

    typedef struct pt pt_t;
    void yyerror(__attribute__((unused)) pt_t** pt, const char *s);
%}

%code requires {
    #include "parsetree.h"
    #include "parser.h"
}

%union {
  int int_val;
  char* lexeme;

  pt_t* pt_type;
  id_list_t* id_list_type;
}

%token FUNC PARAM VAR CALL RETURN IF WHILE
%token PLUS MINUS EQUALS DEQUALS
%token SEMICOLON LPAREN RPAREN LCURLY RCURLY COMMA
%token <lexeme> ID
%token <int_val> NUMBER
%token ERROR

%left EQUALS
%left DEQUALS
%left PLUS MINUS

%type <pt_type> file function_def_list function_def function_name param_def var_def statement_list empty_statement_list statement expr function_call return_statement if_statement while_statement
%type <id_list_type> id_list


%parse-param { pt_t** pt }


%start file
%%

file:
    function_def_list { if(pt != NULL) *pt = $1; }
    ;
function_def_list:
    function_def { $$ = $1; }
    | function_def function_def_list {
        $$ = $1;
        addNext($$, $2);
    }
    ;
function_def:
    FUNC function_name SEMICOLON param_def var_def LCURLY empty_statement_list RCURLY { 
        $$ = allocatePT_TYPE(pt_FUNCTION_DEF);
        addChild($$, $2);
        addChild($$, $4);
        addChild($$, $5);
        addChild($$, $7);
    }
    ;
function_name:
    ID { $$ = allocatePT_STR($1); }
    ;
param_def:
    PARAM id_list SEMICOLON { $$ = allocatePT_ID_LIST(pt_PARAM_DEF, $2); }
    | %empty { $$ = allocatePT_TYPE(pt_PARAM_DEF); }
    ;
var_def:
    VAR id_list SEMICOLON { $$ = allocatePT_ID_LIST(pt_VAR_DEF, $2); }
    | %empty { $$ = allocatePT_TYPE(pt_VAR_DEF); }
    ;
empty_statement_list: 
    statement_list { $$ = $1; }
    | %empty { $$ = allocatePT_TYPE(pt_NONE); }
    ;
statement_list:
    statement { 
        $$ = $1;
    }
    | statement statement_list {
        $$ = $1;
        addNext($$, $2);
    }
    ;
statement:
    expr SEMICOLON { $$ = $1; }
    | return_statement SEMICOLON { $$ = $1; }
    | if_statement { $$ = $1; }
    | while_statement { $$ = $1; }
    | SEMICOLON { $$ = allocatePT_TYPE(pt_NONE); }
    ;
expr:
    NUMBER { $$ = allocatePT_NUM($1); }
    | ID { $$ = allocatePT_STR($1); }
    | function_call { $$ = $1; }
    | expr EQUALS expr {
        $$ = allocatePT_TYPE(pt_EQUALS);
        addChild($$, $1);
        addChild($$, $3);
    }
    | expr DEQUALS expr { 
        $$ = allocatePT_TYPE(pt_DEQUALS);
        addChild($$, $1);
        addChild($$, $3);
    }
    | expr PLUS expr { 
        $$ = allocatePT_TYPE(pt_PLUS);
        addChild($$, $1);
        addChild($$, $3);
    }
    | expr MINUS expr { 
        $$ = allocatePT_TYPE(pt_MINUS);
        addChild($$, $1);
        addChild($$, $3);
    }
    | LPAREN expr RPAREN {
        $$ = $2;
    }
    ;
function_call:
    CALL function_name LPAREN RPAREN {
        $$ = allocatePT_TYPE(pt_CALL);
        addChild($$, $2);
        addChild($$, allocatePT_TYPE(pt_PARAM_LIST));
    }
    | CALL function_name LPAREN id_list RPAREN {
        $$ = allocatePT_TYPE(pt_CALL);
        addChild($$, $2);
        addChild($$, allocatePT_ID_LIST(pt_PARAM_LIST, $4));
    }
    ;
return_statement:
    RETURN expr {
        $$ = allocatePT_TYPE(pt_RETURN);
        addChild($$, $2);
    }
    ;
if_statement:
    IF LPAREN expr RPAREN LCURLY empty_statement_list RCURLY {
        $$ = allocatePT_TYPE(pt_IF);
        addChild($$, $3);
        addChild($$, $6);
    }
    ;
while_statement:
    WHILE LPAREN expr RPAREN LCURLY empty_statement_list RCURLY {
        $$ = allocatePT_TYPE(pt_WHILE);
        addChild($$, $3);
        addChild($$, $6);
    }
    ;
id_list:
    ID { $$ = getIDNode($1, NULL); }
    | ID COMMA id_list { $$ = getIDNode($1, $3); }
    ;
%%

void yyerror(__attribute__((unused)) pt_t** pt, const char *s) {
    fprintf(stderr, "Parse error!  Message: %s on %d\n", s, line_num);
    // might as well halt now:
    exit(-1);
}

pt_t* parse(FILE* fp) {
    yyin = fp;
    pt_t* root = malloc(sizeof(*root));
    yyparse(&root);
    return root;
}
