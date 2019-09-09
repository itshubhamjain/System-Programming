#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 	20		// MAXIMUM SYMBOL TABLE ENTRIES
#define M_MAX 	11		// MAXIMUM MNEMONICS
#define AD_MAX 5		// MAXIMUM ASSEMBLER DIRECTIVES
#define R_MAX 	4		// MAXIMUM REGISTERS
#define C_MAX 	6		// MAXIMUM CONDITION CODES


struct SYMBTAB
{
	char Symb[10];
	int Addr;	// Val for DC, Len for DS
}S[MAX];

struct LITTAB
{
	char Lit[5];
	int Addr;
}L[MAX];

int POOLTAB[MAX],SymbCnt,LitCnt,PoolCnt,TokCnt,LC,i,j,k;
char Tok1[MAX],Tok2[MAX],Tok3[MAX],Tok4[MAX],Tok5[MAX],Buffer[80],temp[MAX];
FILE *fp1,*fp2,*fp;  // fp is for inputfile, fp1 - IC, fp2 - TC

//DATA STRUCTURE INITIALIZATION

char MOTAB[][10]	= { "STOP","ADD","SUB","MULT","MOVER","MOVEM","COMP","BC","DIV","READ","PRINT"};
char REGTAB[][10] 	= { "AREG","BREG","CREG","DREG"};
char CONDTAB[][10] 	= { "LT","LE","EQ","GT","GE","ANY"};
char ADTAB[][10]	= { "START","END","ORIGIN","EQU","LTORG"};

int SearchSymb(char *s)
{
	int i;
	for( i = 0; i < SymbCnt; i++)
	{
		if(strcmp(S[i].Symb,s)==0)
			return(i);
	}
	return -1;
}// END OF SearchSymb()

int SearchLit(char *s)
{
	int i;
	for( i = POOLTAB[PoolCnt]; i < LitCnt; i++)
	{
		if(strcmp(L[i].Lit,s)==0)
			return(i);
	}
	return -1;
}// END OF SearchLit()

int SearchOp(char *s)
{
	int i;
	for( i = 0; i < M_MAX; i++)
	{
		if(strcmp(MOTAB[i],s) == 0)
			return(i);
	}
	return -1;
}// END OF SearchOp()

int SearchReg(char *s)
{
	int i;
	for( i = 0; i < R_MAX; i++)
	{
		if(strcmp(REGTAB[i],s)==0)
			return(i);
	}
	return -1;
}//// END OF SearchReg()

int SearchCond(char *s)
{
	int i;
	for( i = 0; i < C_MAX; i++)
	{
		if(strcmp(CONDTAB[i],s)==0)
			return(i);
	}
	return -1;
}// END OF SearchCond()

int SearchDirective(char *s)
{
	int i;
	for( i = 0; i < 5; i++)
	{
		if(strcmp(ADTAB[i],s)==0)
			return(i);
	}
	return -1;
}// END OF SearchDirective()

void DispSymbTab()
{
	int i;
	//LISTING SYMBOL TABLE
	printf("\n------------------------------------------");
	printf("\nSymb\tAddr\tDecl\tUsed\tVal\tLen");
	printf("\n------------------------------------------");
	for(i=0;i<SymbCnt;i++)
		printf("\n%s\t%d",S[i].Symb,S[i].Addr);
	printf("\n------------------------------------------");

}// END OF DispSymbTab()

void DispLitTab()
{
	int i;
	printf("\n-------------------------------");
	printf("\nLit#\tLit\tAddr");
	printf("\n-------------------------------");
	for(i=0;i<LitCnt;i++)
		printf("\n%02d\t%s\t%d",i,L[i].Lit,L[i].Addr);
	printf("\n-------------------------------");
}// END OF DispLitTab()

void DispPoolTab()
{
	int i;
	printf("\n--------------------");
	printf("\nPool#\tPool Base");
	printf("\n--------------------");
	for(i=0;i<PoolCnt;i++)
		printf("\n%02d\t%d",i,POOLTAB[i]);
	printf("\n--------------------");
}// END OF DispPoolTab()

void PrintFile(char *fn)
{
	fp=fopen(fn,"r");
	if( fp == NULL)
	{
		printf("\nCannot Open File %s",fn);
		exit(0);
	}
	while(fgets(Buffer,80,fp))
		printf("%s",Buffer);
	fclose(fp);
}// END OF PrintFile()

void PassOne()
{
	// fp1 - ptr of source file
	// fp2 - ptr of Intermediate file

	fp1 = fopen("input.ASM","r"); // input file
	if( fp1 == NULL)
	{
		printf("\nCannot Open File ");
		exit(0);
	}

	fp2 = fopen("input.DAT","w");  // IC file
	if( fp2 == NULL)
	{
		printf("\nCannot Create File ");
		exit(0);
	}

	while ( fgets( Buffer, 80, fp1) )
	{
		// INITIALIZE TOKENS TO NULL
		strcpy(Tok1,"");
		strcpy(Tok2,"");
		strcpy(Tok3,"");
		strcpy(Tok4,"");

		TokCnt = sscanf(Buffer,"%s %s %s %s",Tok1,Tok2,Tok3,Tok4);
		switch(TokCnt)
		{
			case 1:  // STOP, START, LTORG, END

				i = SearchOp(Tok1);  
				if(i == 0)	// STOP
				{
					fprintf(fp2,"(IS, %02d)\n",i);
					break;
				}

				i = SearchDirective(Tok1);

				if ( i == 0 )
				{
					fprintf(fp2,"(AD, %02d)\n",i);
					LC = -1;
				}
				else if( i == 1 || i == 4)  // END or LTORG
				{
					fprintf(fp2,"(AD, %02d)\n",i);

					for( k = POOLTAB[PoolCnt]; k < LitCnt; k++)
						L[k].Addr = LC++;

					POOLTAB[++PoolCnt] = LitCnt;
					LC--;
				}
				
				break;

			case 2:  // READ, PRINT, START, ORIGIN, STOP WITH LABEL

				i = SearchOp(Tok1);  // SEARCH IN MOTAB
				if( i == 9 || i == 10) // READ or PRINT
				{
					j = SearchSymb(Tok2);
					if(j == -1) // SYMBOL IS NOT PRESENT
					{
						fprintf(fp2,"(IS, %02d) (S, %02d)\n",i,SymbCnt);
						strcpy(S[SymbCnt++].Symb,Tok2);
					}
					else
					{
						fprintf(fp2,"(IS, %02d) (S, %02d)\n",i,j);
					}
					break;
				}

				i = SearchDirective(Tok1);  // SEARCH IN ADTAB
				if(i == 0 || i == 2)  // START or ORIGIN
				{
					LC = atoi(Tok2)-1;
					fprintf(fp2,"(AD, %02d) (C, %02d)\n",i,LC+1);
					break;
				}
				
				break;

			case 3:
				i = SearchOp(Tok1);
				if(i >= 1 && i <= 8)	// ADD - DIV
				{
					if(Tok3[0] == '=')// Operand is literal
					{
						j = SearchLit(Tok3);
						if( j == -1)//LITERAL NOT PRESENT
						{
							sprintf(temp,"(L, %02d)",LitCnt);
							strcpy(L[LitCnt++].Lit,Tok3);
						}
						else
						{
							sprintf(temp,"(L, %02d)",j);
						}
					}
					else	// Operand is symbol
					{
						j = SearchSymb(Tok3);
						if(j == -1)// SYMBOL NOT PRESENT
						{
							sprintf(temp,"(S, %02d)",SymbCnt);
							strcpy(S[SymbCnt++].Symb,Tok3);
						}
						else
						{
							sprintf(temp,"(S, %02d)",j);
						}
					}// END OF OPERAND IS SYMBOL

					Tok2[strlen(Tok2)-1]='\0';
					if( i == 7) // First operand is reg or cond
						j = SearchCond(Tok2);
					else
						j = SearchReg(Tok2);
					
					fprintf(fp2,"(IS, %02d) (%d) %s\n",i,j,temp);

					break;
				}// END OF 1 TO 8 I.E. ADD - DIV

				if( strcmp(Tok2,"DC") == 0) // DC
				{
					j = SearchSymb(Tok1);
					if(j == -1)// SYMBOL NOT PRESENT
					{
						strcpy(S[SymbCnt].Symb,Tok1);
						S[SymbCnt++].Addr = LC;
					}
					else// SYMBOL PRESENT
					{
						S[j].Addr = LC;						
					}// END OF SYMBOL PRESENT

					fprintf(fp2,"(DL, 00) (C, %02d)\n",atoi(Tok3));
					break;
				}// END OF DC

				if( strcmp(Tok2,"DS") == 0) // DS
				{
					j = SearchSymb(Tok1);
					if( j == -1)// SYMBOL NOT PRESENT
					{
						strcpy(S[SymbCnt].Symb,Tok1);
						S[SymbCnt++].Addr = LC;
					}
					else// SYMBOL PRESENT
					{
						S[j].Addr = LC;						
					}// END OF SYMBOL PRESENT
					LC += atoi(Tok3)-1;
					fprintf(fp2,"(DL, 01) (C, %02d)\n",atoi(Tok3));
					break;
				}// END OF DS

				i = SearchDirective(Tok2);
				if( i == 3)	// EQU
				{
					k = SearchSymb(Tok3);
					j = SearchSymb(Tok1);
					if(j == -1) // NOT PRESENT
					{
						strcpy(S[SymbCnt].Symb,Tok1);
						S[SymbCnt++].Addr = S[k].Addr;
					}
					else// SYMBOL PRESENT
					{
						S[j].Addr = S[k].Addr;
					}// END OF SYMBOL PRESENT
					fprintf(fp2,"(AD, %02d)\n",i);
					LC--; // NO LC PROCESSING FOR EQU
					break;
				}// END OF EQU
				
				break;

			case 4:// INSTRUCTION WITH LABEL
				
					j = SearchSymb(Tok1); // Label
					if(j == -1)// SYMBOL NOT PRESENT
					{
						strcpy(S[SymbCnt].Symb,Tok1);
						S[SymbCnt++].Addr = LC;
					}
					else// PRESENT
					{
						S[j].Addr = LC;
						
					}//END OF SYMBOL PRESENT
					i = SearchOp(Tok2);
					if(i >= 1 && i <= 8)	// ADD - DIV
					{
						if(Tok4[0] == '=')// Operand is literal
						{
							j = SearchLit(Tok4);
							if(j==-1)
							{
								sprintf(temp,"(L, %02d)",LitCnt);
								strcpy(L[LitCnt++].Lit,Tok4);
							}
							else
							{
								sprintf(temp,"(L, %02d)",j);
							}
						}
						else // Operand is symbol
						{
							j = SearchSymb(Tok4);
							if(j==-1)
							{
								sprintf(temp,"(S, %02d)",SymbCnt);
								strcpy(S[SymbCnt++].Symb,Tok4);

							}
							else
							{
								sprintf(temp,"(S, %02d)",j);

							}
						}// END OF OPERAND SYMBOL
						Tok3[strlen(Tok3)-1]='\0';
						if(i == 7) // First operand is reg or cond
							j = SearchCond(Tok3);
						else
							j = SearchReg(Tok3);
						fprintf(fp2,"(IS, %02d) (%d) %s\n",i,j,temp);
				}// END OF ADDDIV									
			break;
		}//END OF SWITCH
	LC++;
	}//END OF WHILE
	fcloseall();
}//END OF PASS-1

void PassTwo()
{
	int i,j,k;
	char temp[MAX];
	LC=0;
	if((fp1=fopen("input.DAT","r"))==NULL)
	{
		//printf("\nCannot Open File %s",InterFile);
		printf("\nCannot Open File ");
		exit(0);
	}
	if((fp2=fopen("input.OBJ","w"))==NULL)
	{
		printf("\nCannot Create Target File ");
		exit(0);
	}
	while(fgets(Buffer,80,fp1))
	{
		TokCnt = sscanf(Buffer,"%s %s %s %s %s",Tok1,Tok2,Tok3,Tok4,Tok5);
		Tok1[strlen(Tok1)-1]='\0';
		Tok2[strlen(Tok2)-1]='\0';
		Tok3[strlen(Tok3)-1]='\0';
		Tok4[strlen(Tok4)-1]='\0';
		Tok5[strlen(Tok5)-1]='\0';
		switch(TokCnt)
		{
			case 2:
				
				if(strcmp(Tok1+1,"AD")==0)	// LTORG, END, EQU
				{
					for(j=0;j<PoolCnt;j++)
						if(L[POOLTAB[j]].Addr==LC)
							break;
					for(i=POOLTAB[j];i<POOLTAB[j+1];i++)
					{
						strcpy(temp,L[i].Lit);
						temp[strlen(temp)-1]='\0';
						fprintf(fp2,"%d) + 00 0 %03d\n",LC++,atoi(strstr(temp,"'")+1));
					}
					LC--;
				}
				else if(strcmp(Tok1+1,"IS")==0)// STOP
				{
					fprintf(fp2,"%d) + 00 0 000\n",LC);
				}
				break;

			case 4:

				if(strcmp(Tok1+1,"AD")==0)	// START, ORIGIN
				{
					LC = atoi(Tok4)-1;
				}
				else if(strcmp(Tok1+1,"IS")==0)	// READ, PRINT
				{
					fprintf(fp2,"%d) + %02d 0 %03d\n",LC,atoi(Tok2),S[atoi(Tok4)].Addr);
				}
				else if(strcmp(Tok1+1,"DL")==0)
				{
					if(atoi(Tok2)==1)		   		// DS
					{
						for(i=0;i<atoi(Tok4);i++)
							fprintf(fp2,"%d)\n",LC++);
						LC--;
					}
					else if(atoi(Tok2)==0)			    	// DC
					{
						fprintf(fp2,"%d) + 00 0 %03d\n",LC,atoi(Tok4));
					}
				}
				break;

			case 5:
				Tok2[strlen(Tok2)-1]='\0';
				Tok3[strlen(Tok3)-1]='\0';
				Tok5[strlen(Tok5)-1]='\0';
				if(Tok4[1]=='S')		// ADD-DIV with symbols
				{
					fprintf(fp2,"%d) + %02d %d %03d\n",LC,atoi(Tok2),atoi(Tok3+1),S[atoi(Tok5)].Addr);
				}
				else if(Tok4[1]=='L')		// ADD-DIV with literals
				{
					fprintf(fp2,"%d) + %02d %d %03d\n",LC,atoi(Tok2),atoi(Tok3+1),L[atoi(Tok5)].Addr);
				}
				break;
		}
		LC++;
	}
	fcloseall();
}

void display_tables()
{
	DispSymbTab();
	//getch();
	DispLitTab();
	//getch();
	DispPoolTab();
	//getch();
}

int main()
{
	//clrscr();
	char in[10] = "input.DAT";
	char in_obj[10] = "input.OBJ";
	PassOne();
	display_tables();
	printf("\nIntermediate Code\n");
	PrintFile(in);
	//getch();

	
		PassTwo();
		printf("\nTarget Code\n");
		PrintFile(in_obj);
	
	
	return 0;
}
