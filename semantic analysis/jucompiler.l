%X COMMENTFIRST
%X COMMENT
%X STRLIT_STATE
%{
	#include "y.tab.h"
	#include "tree.h"
	#include "semantics.h"
	#include "symbol_table.h"
	int yylex(void);
	int line = 1, column = 1, token_column, error_flag = 0, start_line, start_column, start_column_quote;
	int ErrorMode = 0, parseFlag = 0;
	int errors;
	int onlyErrors = 0;
	extern node* root;
	token * a;

%}

digit								[0-9]
letter								[a-zA-Z]
oct									[0][0-9]
hex									[0][x|X][0-9a-fA-F]+
SPACE								" "|"\t"
NEWLINE								\n|\r|\r\n
FORM_FEED							"\f"
ESCAPE_SEQ							\\f|\\n|\\r|\\t|\\\\|\\\"
BOOLLIT 							"true"|"false"
AND 								"&&"
ASSIGN 								"="
STAR 								"*"
COMMA 			 					","
DIV 								"/"
EQ 									"=="
GE 									">="
GT 									">"
LBRACE 								"{"
LE 									"<="
LPAR 								"("
LSQ 		 						"["
LT 									"<"
MINUS 								"-"
MOD 	 							"%"
NE 		 							"!="
NOT 	 							"!"
OR  								"||"
PLUS 	 							"+"
RBRACE 		 						"}"
RPAR  								")"
RSQ 	 							"]"
SEMICOLON  							";"
ARROW  								"->"
LSHIFT  							"<<"
RSHIFT  							">>"
XOR 						 		"^"
BOOL 								"boolean"
CLASS 								"class"
DOTLENGTH  							".length"
DOUBLE 								"double"
ELSE 								"else"
IF  								"if"
INT 								"int"
PRINT 								"System.out.print"
PARSEINT 							"Integer.parseInt"
PUBLIC 								"public"
RETURN 								"return"
STATIC 								"static"
STRING 								"String"
VOID 								"void"
WHILE 								"while"
RESERVED 							("--"|"++"|"null"|"Integer"|"System"|"this"|"private"|"protected"|"abstract"|"continue"|"for"|"new"|"switch"|"assert"|"default"|"package"|"synchronized"|"do"|"goto"|"break"|"implements"|"throw"|"byte"|"import"|"throws"|"case"|"enum"|"instanceof"|"transient"|"catch"|"extends"|"short"|"try"|"char"|"final"|"interface"|"finally"|"long"|"strictfp"|"volatile"|"const"|"float"|"native"|"super")  
ID									([a-zA-Z]|"_"|"$")([0-9]|[a-zA-Z]|"_"|"$")*
INTLIT								([1-9]([0-9]|("_")+[0-9])*)|0
INT_								[0-9](("_"|"_"[0-9])|[0-9])*
EXPOENTE							(("e"|"E")("+"|"-")?{INT_})
REALLIT								({INT_}{EXPOENTE})|("."{INT_}{EXPOENTE})|({INT_}".")|({INT_}"."{EXPOENTE})|({INT_}*"."{INT_}*[0-9]+)|({INT_}"."{INT_}{EXPOENTE})

ILLEGALCHAR							("."|"?"|"#"|":"|"'"|"´"|"`"|"|"|"&"|"\\")
QUOTE								\"
ESCAPE_SEQ_BAR						(\\.|\\) 
STRLIT 								{QUOTE}([^\n\r\"\\]|(\\f|\\n|\\r|\\t|\\\\|\\\"))*{QUOTE}

%%

"/*"								{BEGIN COMMENTFIRST;
									 start_column = column;
									 start_line = line;
									 column += yyleng;}

<COMMENTFIRST>{NEWLINE}				{line += 1;
									column = yyleng;
									token_column = (int)(column-strlen(yytext));}

<COMMENTFIRST><<EOF>>				{printf("Line %d, col %d: unterminated comment\n", start_line, start_column);
									column += yyleng;
									return 0;}

<COMMENTFIRST>.						{column += yyleng;}

<COMMENTFIRST>"*/"					{BEGIN 0;
									column += yyleng;}

"//"								{BEGIN COMMENT;
									column += yyleng;}

<COMMENT>{NEWLINE}					{line += 1;
									 column = 1;
									 BEGIN 0;}

<COMMENT>.							{column += yyleng;}

{STRLIT}						    {if(ErrorMode == 0) {
										printf("STRLIT(%s)\n", yytext);
									} 

									column+=yyleng; 

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column);
										return STRLIT;}
									}


"\""								{BEGIN STRLIT_STATE;
									start_column_quote = column;
									column += yyleng;
									}

<STRLIT_STATE>{ESCAPE_SEQ}			{column += yyleng;}

<STRLIT_STATE>{ESCAPE_SEQ_BAR}		{printf("Line %d, col %d: invalid escape sequence (%s)\n", line, column, yytext); 
									column += yyleng;}

<STRLIT_STATE>{NEWLINE}				{printf("Line %d, col %d: unterminated string literal\n", line, start_column_quote); 
									line += 1; 
									column = 1;
									BEGIN 0;
									}

<STRLIT_STATE><<EOF>>				{printf("Line %d, col %d: unterminated string literal\n", line, start_column_quote);
									column += yyleng;
									token_column = (int)(column-strlen(yytext));
									BEGIN 0;}

<STRLIT_STATE>"\""					{column += yyleng;
									 token_column = (int)(column-strlen(yytext));
									 BEGIN 0;}

<STRLIT_STATE>.						{column += yyleng;
									 token_column = (int)(column-strlen(yytext));}

{NEWLINE}							{column = 1;
									line += 1;}

{FORM_FEED}							{column += 1;
									 token_column = (int)(column-strlen(yytext));}

{BOOLLIT}							{if(ErrorMode == 0)
										printf("BOOLLIT(%s)\n", yytext); 

									column+= yyleng;
									token_column = (int)(column-strlen(yytext));

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column);
										return BOOLLIT;
									}
									}

{AND}								{if(ErrorMode == 0)
										printf("AND\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return AND;
									}
									}

{ASSIGN}							{if(ErrorMode == 0)
										printf("ASSIGN\n");

									column += yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return ASSIGN;
									}
									}

{STAR}								{if(ErrorMode == 0)
										printf("STAR\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return STAR;}
									}

{COMMA}								{if(ErrorMode == 0)
										printf("COMMA\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return COMMA;
									}
									}

{DIV}								{if(ErrorMode == 0)
										printf("DIV\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return DIV;
									}
									}

{EQ}								{if(ErrorMode == 0)
										printf("EQ\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return EQ;}
									}

{GE}								{if(ErrorMode == 0)
										printf("GE\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return GE;
									}
									}

{GT}								{if(ErrorMode == 0)
										printf("GT\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return GT;
									}
									}

{LBRACE}							{if(ErrorMode == 0)
										printf("LBRACE\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return LBRACE;
									}
									}

{LE}								{if(ErrorMode == 0)
										printf("LE\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return LE;
									}
									}

{LPAR}								{if(ErrorMode == 0)
										printf("LPAR\n");

									column+= yyleng;
									token_column = (int)(column-strlen(yytext));

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return LPAR;
									}
									}

{LSQ} 								{if(ErrorMode == 0)
										printf("LSQ\n");

									column+= yyleng;
									token_column = (int)(column-strlen(yytext));

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return LSQ;
									}
									}

{LT} 								{if(ErrorMode == 0)
										printf("LT\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return LT;}
									}

{MINUS} 							{if(ErrorMode == 0)
										printf("MINUS\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return MINUS;}
									}

{MOD} 	 							{if(ErrorMode == 0)
										printf("MOD\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return MOD;}
									}

{NE} 		 						{if(ErrorMode == 0)
										printf("NE\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return NE;}
									}

{NOT} 	 							{if(ErrorMode == 0)
										printf("NOT\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return NOT;}
									}

{OR}  								{if(ErrorMode == 0)
										printf("OR\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return OR;}
									}

{PLUS} 	 							{if(ErrorMode == 0)
										printf("PLUS\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return PLUS;}
									}

{RBRACE} 		 					{if(ErrorMode == 0)
										printf("RBRACE\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return RBRACE;}
									}

{RPAR}  							{if(ErrorMode == 0)
										printf("RPAR\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return RPAR;}
									}

{RSQ} 	 							{if(ErrorMode == 0)
										printf("RSQ\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return RSQ;}
									}

{SEMICOLON}  						{if(ErrorMode == 0)
										printf("SEMICOLON\n");	

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return SEMICOLON;}
									}

{ARROW}  							{if(ErrorMode == 0)
										printf("ARROW\n");

									column+= yyleng;
									token_column = (int)(column-strlen(yytext));

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return ARROW;}
									}

{LSHIFT}  							{if(ErrorMode == 0)
										printf("LSHIFT\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return LSHIFT;}
									}

{RSHIFT}  							{if(ErrorMode == 0)
										printf("RSHIFT\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return RSHIFT;}
									}

{XOR} 						 		{if(ErrorMode == 0)
										printf("XOR\n");

									column+= yyleng;
									token_column = (int)(column-strlen(yytext));

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return XOR;}
									}

{BOOL} 								{if(ErrorMode == 0)
										printf("BOOL\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return BOOL;}
									}

{CLASS} 							{if(ErrorMode == 0)
										printf("CLASS\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return CLASS;}
									}

{DOTLENGTH}  						{if(ErrorMode == 0)
										printf("DOTLENGTH\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return DOTLENGTH;}
									}

{DOUBLE} 							{if(ErrorMode == 0)
										printf("DOUBLE\n");

									column+= yyleng;
									token_column = (int)(column-strlen(yytext));

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return DOUBLE;}
									}

{ELSE} 								{if(ErrorMode == 0)
										printf("ELSE\n");

									column+= yyleng;
									token_column = (int)(column-strlen(yytext));

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return ELSE;}
									}

{IF}  								{if(ErrorMode == 0)
										printf("IF\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return IF;}
									}

{INT} 								{if(ErrorMode == 0) {
										printf("INT\n");
									}

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return INT;}
									}

{PRINT} 							{if(ErrorMode == 0)
										printf("PRINT\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return PRINT;}
									}

{PARSEINT} 							{if(ErrorMode == 0)
										printf("PARSEINT\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return PARSEINT;}
									}

{PUBLIC} 							{if(ErrorMode == 0)
										printf("PUBLIC\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return PUBLIC;}
									}

{RETURN} 							{if(ErrorMode == 0)
										printf("RETURN\n");

									column+= yyleng;

									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return RETURN;}
									}

{STATIC}							{if(ErrorMode == 0)
										printf("STATIC\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return STATIC;}
									}

{STRING} 							{if(ErrorMode == 0)
										printf("STRING\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
                     					return STRING;}
									}

{VOID} 								{if(ErrorMode == 0)
										printf("VOID\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return VOID;}
									}

{WHILE} 							{if(ErrorMode == 0)
										printf("WHILE\n");

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return WHILE;}
									}

{RESERVED}							{if(ErrorMode == 0)
										printf("RESERVED(%s)\n", yytext);

									column+= yyleng;
									
									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
										return RESERVED;}
									}

{INTLIT}							{if(ErrorMode == 0)
										printf("INTLIT(%s)\n", yytext);

									column+= yyleng;
									
									if(parseFlag == 1) {

										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
                    			 		return INTLIT;
                    			 	}
                    			 	}

{ID}								{if(ErrorMode == 0)
										printf("ID(%s)\n", yytext);

									column+= yyleng;

									if(parseFlag == 1){
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
                     					return ID;}
									}

{REALLIT}							{
									if(ErrorMode == 0)
										printf("REALLIT(%s)\n", yytext);

									column+= yyleng;
									token_column = (int)(column-strlen(yytext));
	
									if(parseFlag == 1) {
										yylval.tokenVal = create_token(yytext, line, column-strlen(yytext));
                    			 		return REALLIT;
									}
                    			 	}

{SPACE}								{column += yyleng;
									 token_column = (int)(column-strlen(yytext));}	

<<EOF>>								{column += 1;
									 return 0;}

.									{printf("Line %d, col %d: illegal character (%s)\n", line, column, yytext);
									 column += yyleng;} 

%%

int main(int argc, char **argv) {

	if(argc == 2) {
		if(strcmp(argv[1],"-e1") == 0 ){
			ErrorMode = 1;
			yylex();
		}

		if(strcmp(argv[1],"-e2") == 0 )
		{
			parseFlag = 1;
			ErrorMode = 1;
			yyparse();
		}

		if(strcmp(argv[1], "-l") == 0) {
			yylex();
		}

		if(strcmp(argv[1], "-t") == 0) {
			parseFlag = 1;
			ErrorMode = 1;
			yyparse();
			if(errors == 0) {
				printTree(0, root);
			}
		}


		if(strcmp(argv[1], "-s") == 0) {
			ErrorMode = 1;
			parseFlag = 1;
			onlyErrors = 0;
			yyparse();
			buildTable(root);
			if(errors == 0) {
			}
		}
	}

	else {
		parseFlag = 1;
		ErrorMode = 1;
		onlyErrors = 1;
		yyparse();
		buildTable(root);
	}
	
	return 0;
}

int yywrap() {
	return 1;
}


void yyerror(char *msg) {
	errors = 1;
    printf ("Line %d, col %d: %s: %s\n", line, (int)(column-(int)yyleng), msg, yytext);
}
