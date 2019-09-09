%token ID NUM WHILE
%right '='
%left '+' '-'
%left '*' '/'
%left UMINUS
%%

S : WHILE{while_kw();} '(' E ')'{cond();} E ';'{trueblk();}
  ;
E :V '='{push();} E{codegen_assign();}
  | E '+'{push();} E{codegen();}
  | E '-'{push();} E{codegen();}
  | E '*'{push();} E{codegen();}
  | E '/'{push();} E{codegen();}
  | '(' E ')'
  | '-'{push();} E{codegen_umin();} %prec UMINUS
  | V
  | NUM{push();}
  ;
V : ID {push();}
  ;
%%

#include "lex.yy.c"
#include<ctype.h>
char st[100][10];
int top=0;
char subscript[2]="0";
char temp[2]="t";

int lnum=1;
int start=1;

main()
 {
 printf("Enter the expression : ");
 yyparse();
 }



push()
 {
  strcpy(st[++top],yytext);
 }

codegen()
 {
 strcpy(temp,"t");
 strcat(temp,subscript);
  printf("%s = %s %s %s\n",temp,st[top-2],st[top-1],st[top]);
  top-=2;
 strcpy(st[top],temp);
 subscript[0]++;
 }

codegen_umin()
 {
 strcpy(temp,"t");
 strcat(temp,subscript);
 printf("%s = -%s\n",temp,st[top]);
 top--;
 strcpy(st[top],temp);
 subscript[0]++;
 }

codegen_assign()
 {
 printf("%s = %s\n",st[top-2],st[top]);
 top-=2;
 }



while_kw()
{
printf("L%d: \n",lnum++);
}


cond()
{
 strcpy(temp,"t");
 strcat(temp,subscript);
 printf("%s = not %s\n",temp,st[top]);
 printf("if %s goto L%d\n",temp,lnum);
 subscript[0]++;
 }

trueblk()
{
printf("goto L%d \n",start);
printf("L%d: \n",lnum);
}
