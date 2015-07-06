%{
extern int yylex();
extern void yyerror(const char* e);
%}

%define parse.error verbose // Better error reporting
%debug

%token COMMA SEMICOLON ASSIGN LEFT_PAREN RIGHT_PAREN LEFT_CBRACE RIGHT_CBRACE PLUS MINUS
    MULTIPLY DIVIDE MODULO NOT LESS LESS_EQUAL GREATER GREATER_EQUAL EQUAL NOT_EQUAL AND OR
    IF ELSE WHILE RETURN VOID TYPE INT_LIT CHAR_LIT STRING_LIT ID

%left OR
%left AND
%left EQUAL NOT_EQUAL
%left LESS LESS_EQUAL GREATER GREATER_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT
%nonassoc LEFT_PAREN RIGHT_PAREN

%start program

%%

program :   body
        |   %empty
        ;

body    :   body func_decl
        |   body func_impl
        |   func_decl
        |   func_impl
        ;

func_decl   :   TYPE ID LEFT_PAREN VOID RIGHT_PAREN SEMICOLON
            |   TYPE ID LEFT_PAREN decl_param_list RIGHT_PAREN SEMICOLON
            |   VOID ID LEFT_PAREN VOID RIGHT_PAREN SEMICOLON
            |   VOID ID LEFT_PAREN decl_param_list RIGHT_PAREN SEMICOLON
            ;

func_impl   :   TYPE ID LEFT_PAREN VOID RIGHT_PAREN LEFT_CBRACE stmts RIGHT_CBRACE
            |   TYPE ID LEFT_PAREN impl_param_list RIGHT_PAREN LEFT_CBRACE stmts RIGHT_CBRACE
            |   VOID ID LEFT_PAREN VOID RIGHT_PAREN LEFT_CBRACE stmts RIGHT_CBRACE
            |   VOID ID LEFT_PAREN impl_param_list RIGHT_PAREN LEFT_CBRACE stmts RIGHT_CBRACE
            ;

decl_param_list :   decl_param_list COMMA TYPE
                |   TYPE
                ;

impl_param_list :   impl_param_list COMMA TYPE ID
                |   TYPE ID
                ;

stmts   :   stmt_list
        |   %empty
        ;

stmt    :   assign_stmt
        |   decl_stmt
        |   if_stmt
        |   while_stmt
        |   return_stmt
        |   call_stmt
        |   empty_stmt
        ;

stmt_list   :   stmt_list stmt
            |   stmt
            ;

assign_stmt :   ID ASSIGN expr SEMICOLON
            ;

decl_stmt   :   TYPE decl_id_list SEMICOLON
            ;

decl_id_list    :   decl_id_list COMMA ID
                |   ID
                ;

if_stmt :   IF LEFT_PAREN expr RIGHT_PAREN LEFT_CBRACE stmts RIGHT_CBRACE ELSE LEFT_CBRACE stmts RIGHT_CBRACE
        ;

while_stmt  :   WHILE LEFT_PAREN expr RIGHT_PAREN LEFT_CBRACE stmts RIGHT_CBRACE
            ;

return_stmt :   RETURN expr SEMICOLON
            |   RETURN SEMICOLON
            ;

call_stmt   :   ID LEFT_PAREN exprs RIGHT_PAREN SEMICOLON
            ;

empty_stmt  :   SEMICOLON
            ;

exprs       :   expr_list
            |   %empty
            ;

expr_list   :   expr_list COMMA expr
            |   expr
            ;

expr    :   expr PLUS expr
        |   expr MINUS expr
        |   expr MULTIPLY expr
        |   expr DIVIDE expr
        |   expr MODULO expr
        |   expr LESS expr
        |   expr LESS_EQUAL expr
        |   expr GREATER expr
        |   expr GREATER_EQUAL expr
        |   expr EQUAL expr
        |   expr NOT_EQUAL expr
        |   NOT expr
        |   LEFT_PAREN expr RIGHT_PAREN
        |   expr AND expr
        |   expr OR expr
        |   ID LEFT_PAREN exprs RIGHT_PAREN
        |   INT_LIT
        |   CHAR_LIT
        |   STRING_LIT
        ;

%%
