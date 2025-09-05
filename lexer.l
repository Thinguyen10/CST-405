%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#define YY_DECL int yylex()

int words = 0;
int chars = 0;
int lines = 0;

%}

letter      [a-zA-Z]
digit       [0-9]
ID          {letter}({letter}|{digit})*
TYPE        int|void|float|char|double|long|short
delim       [ \t\n]
NUMBER      {digit}+(\.{digit}+)?(E[+\-]?{digit}+)?
KEYWORD     if|else|while|for|write|writeln|read|return|break
ws          {delim}+

%%
"/*"    				{
							int c;
							while((c = input()) != 0) {
								if(c == '*') {
									if((c = input()) == '/')
										break;
									else
										unput(c);
								}
							}
						}
						

{TYPE} {
    words++; chars += strlen(yytext);
    printf("%s : TYPE\n", yytext);
}
{KEYWORD} {words++;
           chars += strlen(yytext);
           printf("%s : KEYWORD\n", yytext);
        }

"("     {words++; chars += strlen(yytext);
         printf("%s : OPEN PARENTHESIS\n",yytext);
        }
")"     {words++; chars += strlen(yytext);
         printf("%s : CLOSED PARENTHESIS\n",yytext);
        }
"["     {words++; chars += strlen(yytext);
         printf("%s : OPEN SQUARE BRACKET\n",yytext);
        }
"]"     {words++; chars += strlen(yytext);
         printf("%s : CLOSED SQUARE BRACKET\n",yytext);
        }
"=="     {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"!="     {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
">="     {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"<="     {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
">"      {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"<"      {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"&&"     {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"||"     {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"+"      {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"-"      {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"*"      {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"/"      {words++; chars += strlen(yytext);
         printf("%s : BINARY OPERATION\n",yytext);
        }
"++"      {words++; chars += strlen(yytext);
         printf("%s : UNARY OPERATION\n",yytext);
        }
"--"      {words++; chars += strlen(yytext);
         printf("%s : UNARY OPERATION\n",yytext);
        }
"!"     {words++; chars += strlen(yytext);
         printf("%s : UNARY OPERATION\n",yytext);
        }
"{"      {words++; chars += strlen(yytext);
		 printf("%s : OPEN BRACKET\n",yytext);
		}
"}"      {words++; chars += strlen(yytext);
		 printf("%s : CLOSE BRACKET\n",yytext);
		}
","      {words++; chars += strlen(yytext);
		 printf("%s : SEPARATOR\n",yytext);
		}

{ID}	{words++; chars += strlen(yytext);
		 printf("%s : IDENTIFIER\n",yytext);
		}

			
{NUMBER}	{words++; chars += strlen(yytext);
			  printf("%s : NUMBER\n",yytext);
			}
			
";" {words++; chars++; printf("%s : SEMICOLON\n", yytext);}

"=" {words++; chars++; printf("%s : EQ\n", yytext);}

		

\n		{lines++; chars=0;}
[ \t]	{chars++;}
.		{chars++;
         printf("%s : Unrecognized symbol at line %d char %d\n", yytext,lines,chars);
		}
		


%%
int main(int argc, char**argv)
{
	#ifdef YYDEBUG
		yydebug = 1;
	#endif

	printf("Compiler started. \n\n");

	if (argc > 1){
	  if(!(yyin = fopen(argv[1], "r")))
          {
		perror(argv[1]);
		return(1);
	  }
	}
	yylex();
	return 0;

}



    
