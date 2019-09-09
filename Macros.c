/*Program to implement Macro expansion*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct MNT
{
	char MName[10];
	int PP,KP,MDTP,KPDTP;
}mnt[5];

struct KPDTAB
{
	char PName[10],Def[10];
}kpdt[10];

struct MDT
{
	char Opcode[10],Value[35];
}mdt[25];

char PNTAB[10][10],APTAB[10][10];
int  MDT_Ptr,MNT_Ptr,KPDTAB_Ptr;
int  PNTAB_Ptr, APTAB_Ptr, MEC;

char FName[20], TName[20];
char Buffer[80], tok1[35], tok2[35], tok3[35], tok4[35];
char temp_op[40], temp[40], temp1[40], temp2[40], temp3[40];

FILE *fp1, *fp2;

int SearchMNT(char *s)
{
	int i;
	for(i=0; i<MNT_Ptr; i++)
	{
		if(strcasecmp(s,mnt[i].MName)==0)
		{
			return(i);
		}
	}
	return(-1);
}//end of SearchMNT()

int SearchPNTAB(char *s)
{
	int i;
	for(i=0;i<PNTAB_Ptr;i++)
	{
		if(strcasecmp(PNTAB[i],s)==0)
		{
			return(i);
		}
	}
	return(-1);
}//end of SearchPNTAB()

int SearchKPDT(char *s, int n)
{
	int i;
	for( i = mnt[n].KPDTP; i < ( mnt[n].KPDTP + mnt[n].KP ); i++)
	{
		if(strcasecmp(kpdt[i].PName,s)==0)
		{
			return(i);
		}
	}
	return(-1);
}//end of SearchKPDT()

void Print_MNT()
{
	int i;
	printf("\n\nMACRO NAME TABLE\n");
	printf("\n#\tMName\t#PP\t#KP\t#MDTP\tKPDTP");
	for(i=0;i<MNT_Ptr;i++)
	{
		printf("\n%d\t%s\t%d\t%d\t%d\t%d",i,mnt[i].MName,mnt[i].PP,mnt[i].KP,mnt[i].MDTP,mnt[i].KPDTP);
	}
}//end of Print_MNT() 

void Print_KPDTAB()
{
	int i;
	printf("\n\n\nKEYWORD PARAMETER DEFAULT TABLE\n");
	printf("\n#\tPName\tvalue");
	for( i = 0; i < KPDTAB_Ptr; i++)
	{
		printf("\n%d\t%s\t%s",i,kpdt[i].PName,kpdt[i].Def);
	}
}//end of Print_KPDTAB()

void Print_PNTAB(char mname[20])
{
	int i;
	printf("\n\nPARAMETER NAME TABLE FOR MACRO %s", mname);
	
	printf("\n#\tPName");
	for(i = 0; i < PNTAB_Ptr; i++)
	{
		printf("\n%d\t%s", i, PNTAB[i]);
	}
}//end of Print_PNTAB()

void Print_MDT()
{
	int i;
	printf("\n\nMACRO DEFINITION TBL");
	printf("\n#\tOpcode\tOperand");
	for( i = 0; i < MDT_Ptr; i++)
	{
		printf("\n%d\t%s\t%s",i,mdt[i].Opcode,mdt[i].Value);
	}
}//end of Print_MDT()

void printfile(char fn[])
{
	FILE *fp;
	if((fp=fopen(fn,"rb"))==NULL)
	{
		printf("\nCannot open");
		exit(0);
	}
	while(fgets(Buffer,80,fp))
	{
		printf("%s",Buffer);
	}
	fclose(fp);
}//end of printfile 

void Make_KPDTAB_PNTAB(char *s)// s - parameter list
{
	int i=0,pp=0,kp=0,k=0;
	// i for index of temp 
	// pp for #PP & kp for #KP
	// k for search result
	
	if( *s != '\0' ) // Append , to the string s
		strcat(s,",");

	while(*s && *s!='=') // Process Positional Parameters
	{
		if(*s == ',')
		{
			temp[i] = '\0';
			mnt[MNT_Ptr].PP++;
			i = 0;
			k = SearchPNTAB(temp);
			strcpy(PNTAB[PNTAB_Ptr++],temp);
		}
		else if(*s!='&')
		{
			temp[i++] = *s;
		}

		s++;
	}//end of while

	while(*s) // Process Keyword Parameters
	{
		if(*s=='=')
		{
			temp[i]='\0';
			i=0;
			k = SearchPNTAB(temp);
			strcpy(PNTAB[PNTAB_Ptr++],temp);
			strcpy(kpdt[KPDTAB_Ptr].PName,temp);
		}
		else if(*s==',')
		{
			temp[i]='\0';
			mnt[MNT_Ptr].KP++;
			i=0;
			strcpy(kpdt[KPDTAB_Ptr++].Def,temp);
		}
		else if(*s!='&')
		{
			temp[i++]=*s;
		}
		s++;
	}//end of while(*s)
	MNT_Ptr++;
	
}//end of Make_KPDTAB_PNTAB()

void Macro_Expansion(int mnt_index)
{
	int a = -1,b = -1;
	char test[15];

	MEC = mnt[mnt_index].MDTP;
	while(strcasecmp(mdt[MEC].Opcode,"MEND")!=0)
	{
		sscanf(mdt[MEC].Value,"%s %s",tok1,tok2);

		tok1[strlen(tok1)-1]='\0';
		//PUT APPROPRIATE VALUE FOR TOK1		
		if( tok1[0] == '(' ) // IF IT IS IN INTERMEDIATE CODE, GET IT FROM APTABLE 
		{
			a = atoi(strstr(tok1,",")+1);
			sprintf(temp1,"%s",APTAB[a]);
		}
		else
		{
			sprintf(temp1,"%s",tok1);
		}

		//PUT APPROPRIATE VALUE FOR TOK2
		if( tok2[0] == '(' ) // IF IT IS IN INTERMEDIATE CODE, GET IT FROM APTABLE
		{
			tok2[strlen(tok2)-1]='\0';
			b = atoi(strstr(tok2,",")+1);
			sprintf(temp2,"%s",APTAB[b]);
		}
		else
		{
			sprintf(temp2,"%s",tok2);
		}

		sprintf(temp,"%s, %s",temp1,temp2);
		
		if( mdt[MEC].Opcode[0] == '(' )
		{
			strcpy( test, mdt[MEC].Opcode );	
			test[strlen(test)-1]='\0';
			a = atoi(strstr(test,",")+1);
			fprintf(fp2,"\t\t+%s\t%s\n",APTAB[a],temp);	
		}
		else
		{		
			fprintf(fp2,"\t\t+%s\t%s\n",mdt[MEC].Opcode,temp);
		}
		MEC++;
	}//end of while
}//end of Expand()

void tok1_amp()
{
	int k = 0;	
	k = SearchPNTAB(tok1+1);	
	sprintf(temp_op,"(P,%d)",k);//opcode

	//GENERATE IC FOR TOK2
	if( tok2[0] == '&' )	
	{
		tok2[strlen(tok2)-1]='\0';	//REMOVE ,
		k = SearchPNTAB(tok2+1);
		sprintf(temp1,"(P,%d)",k);//tok2
	}	
	else
	{
		tok2[strlen(tok2)-1]='\0';	//REMOVE ,
		sprintf(temp1,"%s", tok2);//tok2
	}

	//GENERATE IC FOR TOK3
	if( tok3[0] == '&' )	
	{
		k = SearchPNTAB(tok3+1);		
		sprintf(temp2,"%s, (P,%d)",temp1,k);//tok3
	}
	else
	{
		sprintf(temp2,"%s %s",temp1,tok3);//tok3
	}
	//UPDATE MDT			
	strcpy(mdt[MDT_Ptr].Opcode,temp_op);			
	strcpy(mdt[MDT_Ptr++].Value,temp2);
}

void tok2_amp()
{
	int k = 0;
	tok2[strlen(tok2)-1]='\0';
	k = SearchPNTAB(tok2+1);
	
	sprintf(temp,"(P,%d)",k);
	k = SearchPNTAB(tok3+1);
	
	sprintf(temp1,"%s, (P,%d)",temp,k);	
	strcpy(mdt[MDT_Ptr].Opcode,tok1);
	strcpy(mdt[MDT_Ptr++].Value,temp1);
}

void Create_APTAB(int mnt_index)
{
		int i=0,j=0,a=0,b=0;
				//COPY KPDTAB DEFAULT VALUE IN APTAB
		for(APTAB_Ptr=mnt[mnt_index].PP,a=mnt[mnt_index].KPDTP; APTAB_Ptr<mnt[mnt_index].PP+mnt[mnt_index].KP; APTAB_Ptr++,a++)
					strcpy(APTAB[APTAB_Ptr],kpdt[a].Def);

		i=j=APTAB_Ptr=0;
		if ( strcmp( tok2, "") != 0 )
			strcat(tok2,",");
		while(tok2[j] && tok2[j]!='=')
		{
			if(tok2[j]==',')
			{
				temp[i]='\0';
				i=0;
				strcpy(APTAB[APTAB_Ptr++],temp);
			}
			else
			{
				temp[i++] = tok2[j];
			}
			j++;
		}//end of while

		if(APTAB_Ptr!=mnt[mnt_index].PP)
		{
			printf("\nPosition Parameters Missing");
			exit(0);
		}
		APTAB_Ptr+=mnt[mnt_index].KP;

		while(tok2[j])
		{
			if(tok2[j]=='=')
			{
				temp[i]='\0';
				i=0;
				b = SearchKPDT(temp,mnt_index);				
			}
			else if(tok2[j]==',')
			{
				temp[i]='\0';
				i=0;
				strcpy(APTAB[mnt[mnt_index].PP+b-mnt[mnt_index].KPDTP],temp);
			}
			else
			{
				temp[i++]=tok2[j];
			}
			j++;
		}//end of while

}// Create_APTAB


void tok_modelstmt()
{
	int k = 0;
	
	k = SearchMNT(tok1); //Is it a Macro call ?
	
	if(k == -1) // Not a MACRO call		
			fprintf(fp2,"%s",Buffer);  //Print output to file
	else //MACRO CALL
	{
		Create_APTAB(k); 		// Create APTAB with k i.e. MNT index
		Macro_Expansion(k);	// Macro Expansion with respective k i.e. MNT index	
	}
}//end  of tok_modelstmt

void tok3_amp()
{
	int k=0;
	k = SearchPNTAB(tok3+1);
	
	sprintf(temp,"(P,%d)",k);
	sprintf(temp1,"%s %s",tok2,temp);
	strcpy(mdt[MDT_Ptr].Opcode,tok1);
	strcpy(mdt[MDT_Ptr++].Value,temp1);
}//end of tok3_amp


int main()
{
	int TokCnt;
	char mname[20];

	if((fp1=fopen("macro_input.asm","r"))==NULL)
	{
	printf("\nCannot Open Source File...");
		exit(0);
	}
	if((fp2=fopen("macro_expansion.txt","w"))==NULL)
	{
		printf("\nCannot Create Intermediate File...");
		exit(0);
	}

	while(fgets(Buffer,80,fp1))
	{
		strcpy(tok1,"");
		strcpy(tok2,"");
		strcpy(tok3,"");
		strcpy(tok4,"");

		TokCnt = sscanf(Buffer,"%s %s %s %s",tok1,tok2,tok3,tok4);

		if(strcasecmp(tok1,"MACRO")==0)
		{
			fgets(Buffer,80,fp1);
			sscanf(Buffer,"%s %s",tok1,tok2);
			strcpy(mname,tok1);
			strcpy(mnt[MNT_Ptr].MName,tok1);
			mnt[MNT_Ptr].KPDTP = KPDTAB_Ptr;
			mnt[MNT_Ptr].MDTP = MDT_Ptr;
			Make_KPDTAB_PNTAB(tok2);
		}

		else if( (strcasecmp(tok1,"MEND") == 0) )
		{
			strcpy(mdt[MDT_Ptr].Opcode,"MEND");
			strcpy(mdt[MDT_Ptr++].Value,"");
			Print_PNTAB(mname);
			PNTAB_Ptr = 0;
		}//end of MEND
		
		else if( tok1[0]=='&')
			tok1_amp();
		else if(tok2[0]=='&')
			tok2_amp();
		else if(tok3[0]=='&')
			tok3_amp();
		else // TokCnt == 3 i.e. macro body w/o '&' and label
			tok_modelstmt();
	}//end of while fgets
	fclose(fp2);
	
	Print_MNT();
	Print_KPDTAB();
	Print_MDT();
	printf("\n\nMACRO WITH EXPANSION::\n\n");
	printfile("macro_expansion.txt");     
	
	fcloseall();  
	return 0;
}//end of main

