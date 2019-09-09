%{
#include<stdio.h>
#include<stdlib.h>
extern int yylex();
void yyerror(char *msg);
%}



%union
{
   int val;
}

%token NUM
%type <val> E T F
%type <val> NUM


%%
S : E  {printf("%d",$1);}
  ;
E : E '+' T  {$$=$1+$3;}
  | E '-' T  {$$=$1-$3;}
  |T {$$=$1;}
  ;
T : T '*' F {$$=$1*$3;}
  | T '/' F {$$=$1/$3;}
  |F {$$=$1;}
  ;
F : '('E')' {$$=$2;}
  |NUM {$$=$1;}
  ;
%%
void yyerror(char *msg){
fprintf(stderr,"%s",msg);
}
 int main (void) {return yyparse ( );}


