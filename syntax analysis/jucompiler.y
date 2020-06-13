%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

void yyerror(char * n);
int yylex(void);

extern int line;
extern int column;

struct node* root = NULL;
struct node* aux = NULL;
struct node* aux2 = NULL;
struct node* aux3 = NULL;
char* auxType;

%}

%token <id> ID STRLIT BOOLLIT INTLIT REALLIT
%token AND ASSIGN STAR COMMA DIV EQ GE GT LBRACE LE LPAR LSQ LT MINUS MOD NE NOT OR PLUS RBRACE RPAR RSQ SEMICOLON ARROW LSHIFT RSHIFT XOR BOOL CLASS DOTLENGTH ELSE IF INT PRINT PARSEINT PUBLIC RETURN STATIC STRING VOID WHILE RESERVED DOUBLE 

%nonassoc IFX
%nonassoc ELSE

%right ASSIGN
%left OR 
%left AND 
%left XOR
%left EQ NE
%left LT GT LE GE
%left LSHIFT RSHIFT
%left PLUS MINUS
%left STAR DIV MOD
%right NOT
%left LPAR RPAR

%type <node> Program ProgramProd FieldDecl FieldDecl2 MethodDecl MethodHeader MethodBody MethodBodyProd 
FormalParams FormalParamsProd VarDecl VarDeclProd Type Statement auxStatement Assignment MethodInvocation 
auxMethodInvocation ParseArgs Expr
%type <node> Expr1

%union{
    int value;
    char* id;
    struct node* node;
}

%% 

Program: 
    CLASS ID LBRACE ProgramProd RBRACE {  root = create_node("Program", NULL);
                                          aux = create_node("Id", $2);   
                                          add_children(root, aux);
                                          add_brother(aux, $4);
                                        }
    ;

ProgramProd:
    %empty                              { $$ = NULL; }
    | ProgramProd MethodDecl            { if( $1!= NULL )
                                            { $$ = $1; 
                                              add_brother($$, $2); }
                                          else
                                              $$ = $2; }
    | ProgramProd FieldDecl            { if( $1!= NULL )
                                            { $$ = $1; 
                                              add_brother($$, $2); }
                                          else
                                              $$ = $2; }
    | ProgramProd SEMICOLON            { $$ = $1; }
    ;

MethodDecl:
    PUBLIC STATIC MethodHeader MethodBody { $$ = create_node("MethodDecl", NULL);  
                                            add_children($$, $3);
                                            add_brother($3, $4); 
                                            }
    
FieldDecl:
    PUBLIC STATIC Type ID FieldDecl2 SEMICOLON { $$ = create_node("FieldDecl", NULL);
                                                 add_children($$, $3);
                                                 aux = create_node("Id",$4);
                                                 add_brother($3, aux);
                                                 add_brother($$, $5);
                                                 add_type($5, $3);
                                                 }
    | error SEMICOLON                           {$$ = create_node("Error", NULL);}
    ;

FieldDecl2:
    FieldDecl2 COMMA ID { aux2 = create_node("FieldDecl",NULL);
                          if($1 != NULL){
                            add_brother($1, aux2);
                            aux = create_node("Id",$3);
                            add_children(aux2, aux); 
                            $$ = $1;}
                          else{
                            aux = create_node("Id",$3);
                            add_children(aux2, aux); 
                            $$ = aux2;}
                        }
    | %empty            { $$ = NULL; }
    ;
    

Type:
    BOOL  { $$ = create_node("Bool",NULL); }

    | INT { $$ = create_node("Int",NULL);  }

    | DOUBLE { $$ = create_node("Double", NULL); }

    ;

MethodHeader: 
      Type ID LPAR FormalParams RPAR { $$ = create_node("MethodHeader",NULL);
                                       add_children($$,$1);
                                       aux = create_node("Id",$2);
                                       add_brother($1, aux);
                                       add_brother(aux, $4);
                                       }
    | VOID ID LPAR FormalParams RPAR { $$ = create_node("MethodHeader",NULL);
                                       aux = create_node("Void",NULL);
                                       add_children($$,aux);
                                       aux2 = create_node("Id",$2);
                                       add_brother(aux,aux2);
                                       add_brother(aux2,$4);
                                      }
    | Type ID LPAR RPAR              { $$ = create_node("MethodHeader",NULL);
                                       add_children($$,$1);
                                       aux = create_node("Id", $2);
                                       add_brother($1, aux);
                                       aux2 = create_node("MethodParams",NULL); 
                                       add_brother(aux, aux2);}

    | VOID ID LPAR RPAR              { $$ = create_node("MethodHeader",NULL);
                                       aux = create_node("Void", NULL);
                                       add_children($$,aux);
                                       aux2 = create_node("Id",$2);    
                                       add_brother(aux,aux2);
                                       aux3 = create_node("MethodParams",NULL); 
                                       add_brother(aux2, aux3);}
    ; 

FormalParams:
    Type ID FormalParamsProd { $$ = create_node("MethodParams",NULL);
                               aux = create_node("ParamDecl",NULL);
                               add_brother(aux, $3);
                               add_children($$, aux);
                               add_children(aux, $1); 
                               aux = create_node("Id",$2);
                               add_brother($1,aux); 
                                }

    | STRING LSQ RSQ ID      { $$ = create_node("MethodParams",NULL);
                               aux = create_node("ParamDecl",NULL);
                               add_children($$,aux);
                               aux2 = create_node("StringArray",NULL);
                               add_children(aux,aux2);
                               aux3 = create_node("Id",$4);
                               add_brother(aux2,aux3); 
                               }
    ;

FormalParamsProd:
     FormalParamsProd COMMA Type ID {   aux2 = create_node("ParamDecl",NULL);
                                        if($1 != NULL){
                                          add_brother($1, aux2);
                                          add_children(aux2, $3);
                                          aux = create_node("Id", $4);
                                          add_brother($3, aux);
                                          $$ = $1;
                                      }
                                      else{
                                        add_children(aux2, $3);
                                        aux = create_node("Id", $4);
                                        add_brother($3, aux);
                                        $$ = aux2;
                                      }
                                    }

    | %empty                        {$$ = NULL;}
    ;

 
MethodBody:
    LBRACE MethodBodyProd RBRACE    {$$ = create_node("MethodBody", NULL);
                                     add_children($$, $2);}
    ;

MethodBodyProd:
    MethodBodyProd VarDecl {     if($1 != NULL){
                                  add_brother($1,$2);
                                 }
                                 else{
                                  $$ = $2;
                                 }
                                }
    | MethodBodyProd Statement { if($1 != NULL){
                                  add_brother($1,$2);
                                 }
                                 else{
                                  $$ = $2;
                                 }
                                }

    | %empty               { $$ = NULL; }
    ;
 
VarDecl:
    Type ID VarDeclProd SEMICOLON { $$ = create_node("VarDecl",NULL);
                                    add_children($$,$1);
                                    add_type($3,$1);
                                    aux = create_node("Id",$2);
                                    add_brother($1,aux);
                                    add_brother($$,$3);
                                    }
    ;

 VarDeclProd:
     VarDeclProd COMMA ID { aux2 = create_node("VarDecl",NULL);
                            if($1 != NULL){
                              aux = create_node("Id",$3);
                              add_children(aux2, aux);                                              
                              add_brother($1, aux2);
                            }
                            else{
                              aux = create_node("Id",$3);                                              
                              add_children(aux2, aux);
                              $$ = aux2;
                            }
                          }
    | %empty              { $$ = NULL; }
    ;

 
Statement:
    LBRACE auxStatement RBRACE                   {    if(count_brother($2) >= 2){
                                                       $$ = create_node("Block", NULL);                
                                                       add_children($$, $2);
                                                       }
                                                    else if (count_brother($2) == 0) {
                                                        $$ = NULL;
                                                    }
                                                    else {
                                                        $$ = $2;
                                                    }
                                                }
    | IF LPAR Expr RPAR Statement %prec IFX      { $$ = create_node("If",NULL);
                                                   add_children($$,$3);
                                                   if($5 == NULL){
                                                    aux = create_node("Block",NULL);
                                                    add_brother($3,aux);
                                                   }
                                                   else{
                                                    add_brother($3,$5);
                                                   }
                                                   aux = create_node("Block",NULL);
                                                   add_brother($3,aux);
                                                  }
    | IF LPAR Expr RPAR Statement ELSE Statement { $$ = create_node("If",NULL);
                                                   add_children($$,$3);
                                                   if($5 == NULL){
                                                    aux = create_node("Block",NULL);
                                                    add_brother($3,aux);
                                                   }
                                                   else{
                                                    add_brother($3,$5);
                                                   }
                                                   if($7 == NULL){
                                                     aux = create_node("Block",NULL);
                                                     add_brother($3,aux);
                                                   }
                                                   else{
                                                    add_brother($3,$7);
                                                   }
                                                  }
    | WHILE LPAR Expr RPAR Statement             { 
                                                   $$ = create_node("While",NULL);
                                                   add_children($$,$3);
                                                   if($5 == NULL){
                                                    aux = create_node("Block",NULL);
                                                    add_brother($3,aux);
                                                   }
                                                   else{
                                                    add_brother($3,$5);
                                                   }
                                                 }

    | RETURN Expr SEMICOLON                      { $$ = create_node("Return",NULL);
                                                   add_children($$,$2); }
    | RETURN SEMICOLON                           { $$ = create_node("Return",NULL); }

    | MethodInvocation SEMICOLON                 { $$ = $1; }

    | Assignment SEMICOLON                       { $$ = $1; }

    | ParseArgs SEMICOLON                        { $$ = $1; }

    | SEMICOLON                                  { $$ = NULL;}

    | PRINT LPAR Expr RPAR SEMICOLON             { $$ = create_node("Print",NULL);
                                                   add_children($$,$3); }
    | PRINT LPAR STRLIT RPAR SEMICOLON           { $$ = create_node("Print",NULL);
                                                   aux = create_node("StrLit",$3);
                                                   add_children($$,aux); }

    | error SEMICOLON                            {$$ = create_node("Error", NULL);}
    ;

auxStatement:
    %empty                                  { $$ = NULL; }
    | auxStatement Statement                { 
                                                if ($1 != NULL) {
                                                    add_brother($1,$2);
                                                }
                                                else {
                                                    $$ = $2;
                                                }
                                            }
    ;

MethodInvocation:

    ID LPAR  Expr auxMethodInvocation RPAR  { $$ = create_node("Call", NULL);
                                              aux = create_node("Id", $1);
                                              add_children($$, aux); 
                                              add_brother(aux, $3);
                                              add_brother($3, $4); 
                                              }
    | ID LPAR RPAR                          { $$ = create_node("Call",NULL);
                                              aux = create_node("Id",$1);
                                              add_children($$,aux); }
    | ID LPAR error RPAR                    { $$ = create_node("Error", NULL);}
    ;

auxMethodInvocation:
    auxMethodInvocation COMMA Expr          { if($1 != NULL) {
                                                add_brother($1, $3); 
                                                $$ = $1;
                                              }
                                              else
                                                $$ = $3;
                                            }

    | %empty                                { $$ = NULL;}
    ;

Assignment:
    ID ASSIGN Expr                          { $$ = create_node("Assign", NULL); 
                                              aux = create_node("Id", $1);
                                              add_children($$, aux); 
                                              add_brother(aux, $3); }
    ;

ParseArgs:
    PARSEINT LPAR ID LSQ Expr RSQ RPAR { $$ = create_node("ParseArgs", NULL);
                                         aux = create_node("Id", $3);
                                         add_children($$, aux); 
                                         add_brother(aux, $5); }

    | PARSEINT LPAR error RPAR          {$$ = create_node("Error", NULL);}
    ;

Expr:
      Assignment                       { $$ = $1; }
    | Expr1                            { $$ = $1; }
    ;

Expr1: 

      Expr1 PLUS Expr1          { $$ = create_node("Add",NULL);
                                  add_children($$,$1);
                                  add_brother($1,$3); }

    | Expr1 MINUS Expr1         { $$ = create_node("Sub",NULL);
                                  add_children($$,$1);
                                  add_brother($1,$3); }

    | Expr1 STAR Expr1          { $$ = create_node("Mul",NULL);
                                  add_children($$,$1);
                                  add_brother($1,$3); }

    | Expr1 DIV Expr1           { $$ = create_node("Div",NULL);
                                  add_children($$,$1);
                                  add_brother($1,$3); }

    | Expr1 MOD Expr1           { $$ = create_node("Mod",NULL);
                                  add_children($$,$1);
                                  add_brother($1,$3); }

    | Expr1 AND Expr1           { $$ = create_node("And",NULL);
                                  add_children($$,$1);
                                  add_brother($1,$3); }

    | Expr1 OR Expr1            { $$ = create_node("Or",NULL);
                                  add_children($$,$1);
                                  add_brother($1,$3); }

    | Expr1 XOR Expr1           { $$ = create_node("Xor",NULL);
                                  add_children($$,$1);
                                  add_brother($1,$3); }

    | PLUS Expr1 %prec NOT      { $$ = create_node("Plus", NULL);
                                  add_children($$, $2); }

    | MINUS Expr1 %prec NOT     { $$ = create_node("Minus" ,NULL);
                                  add_children($$, $2); }

    | NOT Expr1                 { $$ = create_node("Not", NULL);
                                  add_children($$, $2); }  

    | Expr1 LSHIFT Expr1        { $$ = create_node("Lshift", NULL);
                                  add_children($$, $1);
                                  add_brother($1, $3); }

    | Expr1 RSHIFT Expr1        { $$ = create_node("Rshift", NULL);
                                  add_children($$, $1);
                                  add_brother($1, $3); }

    | Expr1 EQ Expr1            { $$ = create_node("Eq", NULL);
                                  add_children($$, $1);
                                  add_brother($1, $3); }

    | Expr1 GE Expr1            { $$ = create_node("Ge", NULL);
                                  add_children($$, $1);
                                  add_brother($1, $3); }

    | Expr1 GT Expr1            { $$ = create_node("Gt" ,NULL);
                                  add_children($$, $1);
                                  add_brother($1, $3); }

    | Expr1 LE Expr1            { $$ = create_node("Le", NULL);
                                  add_children($$, $1);
                                  add_brother($1, $3); }

    | Expr1 LT Expr1            { $$ = create_node("Lt", NULL);
                                  add_children($$, $1);
                                  add_brother($1, $3); }

    | Expr1 NE Expr1            { $$ = create_node("Ne", NULL);
                                  add_children($$, $1);
                                  add_brother($1, $3); }

    | LPAR Expr RPAR            {$$ = $2;}

    | MethodInvocation          {$$ = $1;}

    | ParseArgs                 {$$ = $1;}

    | ID DOTLENGTH              { $$ = create_node("Length", NULL);
                                  aux = create_node("Id", $1);
                                  add_children($$, aux); }  

    | ID                        { $$ = create_node("Id",$1);}

    | INTLIT                    { $$ = create_node("DecLit", $1);}

    | REALLIT                   { $$ = create_node("RealLit", $1);}

    | BOOLLIT                   { $$ = create_node("BoolLit", $1);}

    | LPAR error RPAR           {$$ = create_node("Error", NULL);}

    ;
 
%%