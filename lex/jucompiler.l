%X COMMENTFIRST
%X COMMENT
%X STRLIT_STATE
%{
	int line = 1, column = 1, error_flag = 0, start_line, start_column, start_column_quote;
	int ErrorMode = 0;
	char aux[100];
	
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
LSQ 								"["
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
STRLIT								"\""(([a-zA-Z]|" ")+|{ESCAPE_SEQ}+)"\""
ILLEGALCHAR							("."|"?"|"#"|":"|"'"|"´"|"`"|"|"|"&"|"\\")
QUOTE								\"
ESCAPE_SEQ_BAR						(\\.|\\) 

%%

"/*"								{BEGIN COMMENTFIRST;
									start_column = column;
									start_line = line;
									column += yyleng;}
<COMMENTFIRST>{NEWLINE}				{line += 1;
									column = yyleng;}

<COMMENTFIRST><<EOF>>				{printf("Line %d, col %d: unterminated comment\n", start_line, start_column);
									yyterminate();}
<COMMENTFIRST>.						{column+=yyleng;}
<COMMENTFIRST>"*/"					{BEGIN 0;
									column += yyleng;}

"//"								{BEGIN COMMENT;
									column += yyleng;}
<COMMENT>{NEWLINE}					{BEGIN 0;
									line += 1;
									column = 1;}

<COMMENT>.							{column += yyleng;}



{QUOTE}								{BEGIN STRLIT_STATE;
									start_column_quote = column;
									column += yyleng;
									strcat(aux, yytext);}

<STRLIT_STATE>{ESCAPE_SEQ}			{column += yyleng;
									strcat(aux, yytext);}

<STRLIT_STATE>{ESCAPE_SEQ_BAR}		{printf("Line %d, col %d: invalid escape sequence (%s)\n", line, column, yytext); 
									column += yyleng;
									error_flag = 1;}

<STRLIT_STATE>{NEWLINE}				{printf("Line %d, col %d: unterminated string literal\n", line, start_column_quote); 
									line += 1; 
									column = 1;
									BEGIN 0;
									error_flag = 0;
									strcpy(aux, "");}

<STRLIT_STATE><<EOF>>				{printf("Line %d, col %d: unterminated string literal\n", line, start_column_quote);
									BEGIN 0;
									error_flag = 0;
									strcpy(aux, "");}

<STRLIT_STATE>{QUOTE}				{BEGIN 0; 
									if (error_flag == 0 && ErrorMode == 0)
										printf("STRLIT(%s%s)\n", aux, yytext); 
									error_flag = 0;
									column += yyleng;
									strcpy(aux, "");}

<STRLIT_STATE>.						{strcat(aux, yytext);
									column += yyleng;}

{NEWLINE}							{column = 1;
									line += 1;}

{FORM_FEED}							{column += 1;}

{BOOLLIT}							{if(ErrorMode == 0)
										printf("BOOLLIT(%s)\n", yytext); 
									column+= yyleng;}

{AND}								{if(ErrorMode == 0)
										printf("AND\n");
									column+= yyleng;}

{ASSIGN}							{if(ErrorMode == 0)
										printf("ASSIGN\n");
									column+= yyleng;}

{STAR}								{if(ErrorMode == 0)
										printf("STAR\n");
									column+= yyleng;}

{COMMA}								{if(ErrorMode == 0)
										printf("COMMA\n");
									column+= yyleng;}

{DIV}								{if(ErrorMode == 0)
										printf("DIV\n");
									column+= yyleng;}

{EQ}								{if(ErrorMode == 0)
										printf("EQ\n");
									column+= yyleng;}

{GE}								{if(ErrorMode == 0)
										printf("GE\n");
									column+= yyleng;}

{GT}								{if(ErrorMode == 0)
										printf("GT\n");
									column+= yyleng;}

{LBRACE}							{if(ErrorMode == 0)
										printf("LBRACE\n");
									column+= yyleng;}

{LE}								{if(ErrorMode == 0)
										printf("LE\n");
									column+= yyleng;}

{LPAR}								{if(ErrorMode == 0)
										printf("LPAR\n");
									column+= yyleng;}

{LSQ} 								{if(ErrorMode == 0)
										printf("LSQ\n");
									column+= yyleng;}

{LT} 								{if(ErrorMode == 0)
										printf("LT\n");
									column+= yyleng;}

{MINUS} 							{if(ErrorMode == 0)
										printf("MINUS\n");
									column+= yyleng;}

{MOD} 	 							{if(ErrorMode == 0)
										printf("MOD\n");
									column+= yyleng;}

{NE} 		 						{if(ErrorMode == 0)
										printf("NE\n");
									column+= yyleng;}

{NOT} 	 							{if(ErrorMode == 0)
										printf("NOT\n");
									column+= yyleng;}

{OR}  								{if(ErrorMode == 0)
										printf("OR\n");
									column+= yyleng;}

{PLUS} 	 							{if(ErrorMode == 0)
										printf("PLUS\n");
									column+= yyleng;}

{RBRACE} 		 					{if(ErrorMode == 0)
										printf("RBRACE\n");
									column+= yyleng;}

{RPAR}  							{if(ErrorMode == 0)
										printf("RPAR\n");
									column+= yyleng;}

{RSQ} 	 							{if(ErrorMode == 0)
										printf("RSQ\n");
									column+= yyleng;}

{SEMICOLON}  						{if(ErrorMode == 0)
										printf("SEMICOLON\n");	
									column+= yyleng;}

{ARROW}  							{if(ErrorMode == 0)
										printf("ARROW\n");
									column+= yyleng;}

{LSHIFT}  							{if(ErrorMode == 0)
										printf("LSHIFT\n");
									column+= yyleng;}

{RSHIFT}  							{if(ErrorMode == 0)
										printf("RSHIFT\n");
									column+= yyleng;}

{XOR} 						 		{if(ErrorMode == 0)
										printf("XOR\n");
									column+= yyleng;}

{BOOL} 								{if(ErrorMode == 0)
										printf("BOOL\n");
									column+= yyleng;}

{CLASS} 							{if(ErrorMode == 0)
										printf("CLASS\n");
									column+= yyleng;}

{DOTLENGTH}  						{if(ErrorMode == 0)
										printf("DOTLENGTH\n");
									column+= yyleng;}

{DOUBLE} 							{if(ErrorMode == 0)
										printf("DOUBLE\n");
									column+= yyleng;}

{ELSE} 								{if(ErrorMode == 0)
										printf("ELSE\n");
									column+= yyleng;}

{IF}  								{if(ErrorMode == 0)
										printf("IF\n");
									column+= yyleng;}

{INT} 								{if(ErrorMode == 0)
										printf("INT\n");
									column+= yyleng;}

{PRINT} 							{if(ErrorMode == 0)
										printf("PRINT\n");
									column+= yyleng;}

{PARSEINT} 							{if(ErrorMode == 0)
										printf("PARSEINT\n");
									column+= yyleng;}

{PUBLIC} 							{if(ErrorMode == 0)
										printf("PUBLIC\n");
									column+= yyleng;}

{RETURN} 							{if(ErrorMode == 0)
										printf("RETURN\n");
									column+= yyleng;}

{STATIC}							{if(ErrorMode == 0)
										printf("STATIC\n");
									column+= yyleng;}

{STRING} 							{if(ErrorMode == 0)
										printf("STRING\n");
									column+= yyleng;}

{VOID} 								{if(ErrorMode == 0)
										printf("VOID\n");
									column+= yyleng;}

{WHILE} 							{if(ErrorMode == 0)
										printf("WHILE\n");
									column+= yyleng;}

{RESERVED}							{if(ErrorMode == 0)
										printf("RESERVED(%s)\n", yytext);
									column+= yyleng;}

{INTLIT}							{if(ErrorMode == 0)
										printf("INTLIT(%s)\n", yytext);
									column+= yyleng;}

{ID}								{if(ErrorMode == 0)
										printf("ID(%s)\n", yytext);
									column+= yyleng;}

{REALLIT}							{
									if(ErrorMode == 0)
										printf("REALLIT(%s)\n", yytext);
									column+= yyleng;}

{SPACE}								{column += yyleng;}	

.									{printf("Line %d, col %d: illegal character (%s)\n", line, column, yytext);
									column += yyleng;} \

%%

int main(int argc, char *argv[]) {
	if(strcmp(argv[1],"-e1") == 0)
		ErrorMode = 1;
	yylex();
	return 0;
}

int yywrap() {
	return 1;
}