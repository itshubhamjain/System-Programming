//Implementation of RE to DFA

#include<stdio.h>
#define MAX 20
#include<malloc.h>


typedef struct tree     //  tree  structure
{
	struct tree * l,*r;
	char c;
	int pos,nul,fpos[MAX],lpos[MAX];
}T;

typedef struct stack   //  stack  structure
{
	struct stack * next;
	struct tree *  t;
}S;

typedef struct followpos  //  followpos  structure
{
	int pos,fp[MAX];
	char sym;
}FP;

typedef struct state   //  state  structure
{
	int set[MAX],seta[MAX],setb[MAX];
		
}ST;

FP  f[MAX];
int n;

void initialise(ST *st)
{
	int i;	
	for(i=0;i<MAX;i++)
		st[i].set[0]=st[i].seta[0]=st[i].setb[0]=0;
}

T * getnode_t()   // memory allocation for tree node
{
	T * temp;
	temp = (T *)(malloc(sizeof(T)));
	temp->l=temp->r=NULL;
	return temp;
}

S * getnode_s()   // memory allocation for stack node
{
	S * temp;
	temp = (S *)(malloc(sizeof(S)));
	temp->next=NULL;
	return temp;
}

int isempty(S *temp) // checking whether stack is empty 
{
	if(temp == NULL)
		return 0;
	return 1;
}

S * push(S * top,T * temp) //pushing on to the stack  
{
	S * n;
	n = getnode_s();
	n->t = temp;
	if(top == NULL)
	{
		top = n;
		return top;
	}
	else
	{
		n->next = top;
		return n;
	}
}

T * pop(S **top)   //poping the top of stack

{
	if(isempty((*top)))
	{
		S * temp;
		T * n;
		temp=(*top);
		n=temp->t;
		(*top)=(*top)->next;
		free(temp);
		return n;
	}
	else
	{
		printf("STACK EMPTY!!!!!");
	}
}
void inorder_s(T *t,int n,FP f[])  // inorder traversal
{
	int i;	
	if(t!=NULL)
	{
		inorder_s(t->l,n,f);
		if(t->l==NULL&&t->r==NULL)
		for(i=0;i<n;i++)
		{
			if(f[i].pos==t->pos)
			{
				f[i].sym=t->c;
				break;
			}
		}
		inorder_s(t->r,n,f);
	}
}
void inorder(T *t)   // inorder traversal for printing firstpos and lastpos
{
	int k;
	if(t != NULL)
	{

		inorder(t->l);
		printf("\n%c ( " ,t->c);
		printf(" pos = %d,nullable = %d) ",t->pos,t->nul);

		k=0;
		printf("\t");
		while(k < (t->fpos[0]))
		{
			printf("%d ",t->fpos[k+1]);
			k++;
		}
		k=0;
		printf("\t\t");
		while(k < (t->lpos[0]))
		{
			printf("%d ",t->lpos[k+1]);
			k++;
		}
		inorder(t->r);
	}
}//end of inorder

void lcopy_fpos(T *t)  //copying the fpos of left child
{
	int k,j;
	k=0;
	j=1;
	while(k<(t->l->fpos[0]))
	{
		t->fpos[j] = (t->l->fpos[j]);
		k++;
		j++;
	}
	t->fpos[0]=j-1;
}

void lcopy_lpos(T *t)  //copying the lpos of left child
{
	int k,j;
	k=0;
	j=1;
	while(k<(t->l->lpos[0]))
	{
		t->lpos[j] = (t->l->lpos[j]);
		k++;
		j++;
	}
	t->lpos[0]=j-1;
}

void rcopy_lpos(T *t) //copying the lpos of right child
{
	int k,j;
	k=0;
	j=1;
	while(k<(t->r->lpos[0]))
	{
		t->lpos[j] = (t->r->lpos[j]);
		k++;
		j++;
	}
	t->lpos[0]=j-1;
}

void lrcopy_fpos(T *t) //copying the fpos of both left and right child
{
	int j,k,l;
	k=0;
	j=1;
	while(k<(t->l->fpos[0]))
	{
		t->fpos[j] = (t->l->fpos[j]);
		k++;
		j++;
	}
	k=0;
	l=1;
	while(k<(t->r->fpos[0]))
	{
		t->fpos[j] = (t->r->fpos[l]);
		j++;
		k++;
		l++;
	}
	t->fpos[0]=j-1;
}


void lrcopy_lpos(T *t) //copying the fpos of both left and right child
{
	int j,k,l;
	k=0;
	j=1;
	while(k<(t->l->lpos[0]))
	{
		t->lpos[j] = (t->l->lpos[j]);
		k++;
		j++;
	}
	k=0;
	l=1;
	while(k<(t->r->lpos[0]))
	{
		t->lpos[j] = (t->r->lpos[l]);
		j++;
		k++;
		l++;
	}
	t->lpos[0]=j-1;
}

void FIRSTPOS( T *t )
{
	if(t->c == '+')
		lrcopy_fpos(t);
	else if(t->c=='.')
	{
		if(t->l->nul==1)
			lrcopy_fpos(t);
		else
			lcopy_fpos(t);

	}
	else if(t->c=='*')
		lcopy_fpos(t);
}

void LASTPOS( T *t )
{
	if(t->c == '+')
		lrcopy_lpos(t);
	else if(t->c=='.')
	{
		if(t->r->nul==1)
			lrcopy_lpos(t);		
		else
			rcopy_lpos(t);		
	}
		
	else if(t->c=='*')
		lcopy_lpos(t);
}

void NULLABLE( T *t)
{
	if(t->c == '*')
		t->nul=1;
	else if(t->c == '.')
	{
		if(((t->l)->nul == 1) && ((t->r)->nul == 1))
		{
			t->nul=1;
		}
		else
		t->nul=0;
	}
	else if(t->c == '+')
	{
		if(((t->l)->nul == 1) || ((t->r)->nul == 1))
			t->nul=1;
		else
			t->nul=0;
	}
	else if(t->c == 'Z')  // for epsilon
		t->nul=1;
	else
		t->nul=0;
}

int check_final(int *a,int final)  //check whether the state is final
{
	int i;
	for(i=1;i<=a[0];i++)
	{	
		if(a[i]==final)
			return 1;

	}
	
	return 0;
}

int is_newstate(ST *s,int *b,int n)  //checkind whether it's a new sate
{
	int i,j,k,flag,count;
	for(i=0;i<n;i++)
	{
		count=0;		
		if(b[0]!=s[i].set[0])
			continue;		
		for(j=1;j<=b[0];j++)
		{
			flag=0;			
 			for(k=1;k<=s[i].set[0];k++)
			{
				if(b[j]==s[i].set[k])
				{
					flag=1;
					count++;
				}
			}
			if(flag!=1)
				j=b[0];
						
		}
		if(count==b[0])
			return 0;
	}
	return 1;
}

void union1(int *a,int *b)  //union of the states
{
	int i,j,flag,k=0;	
	for(i=1;i<=b[0];i++)
	{
		flag=0;		
		for(j=1;j<=a[0];j++)
		{
			if(a[j]==b[i])
			{				
				flag=1;				
				break;
			}
		}
		if(flag!=1)
		{
			a[a[0]+k+1]=b[i];
			k++;
		}		
	}
	a[0]=a[0]+k;
}		

void FOLLOWPOS(T *t)
{
	int k, flag, i_lpos, dup,j ;
	
	if(t->c == '.')
	{
		k = 1;			
		for( i_lpos = 0; i_lpos < (t->l)->lpos[0]; i_lpos++)
		{
			flag = 0;				
			for( dup = 0; dup < n; dup++)
			{
				if( f[dup].pos == (t->l)->lpos[k])
				{
					flag = 1;
					break;
				}	
			}//for dup
			if(flag != 1)
			{				
				f[n].pos = (t->l)->lpos[k];
			
				for(j = 0;j < (t->r)->fpos[0];j++)				
					f[n].fp[j+1] = (t->r)->fpos[j+1];
				f[n].fp[0] = (t->r)->fpos[0];
				n++;
			}
			else
			{
				for(j = 0;j < (t->r)->fpos[0];j++)				
					f[dup].fp[f[dup].fp[0]+j+1] = (t->r)->fpos[j+1];
				f[dup].fp[0] += (t->r)->fpos[0];
							
			}	
			k++;
		}	
	}//end of if concatenation
	
	if(t->c=='*')
	{
		k=1;			
		for(i_lpos = 0; i_lpos < t->lpos[0]; i_lpos++)
		{ 
			flag=0;				
			for(dup = 0;dup < n; dup++)
			{
				if(f[dup].pos == t->lpos[k])
				{
					flag = 1;
					break;
				}
			}//for dup

			if(flag != 1)
			{
				f[n].pos = t->lpos[k];
				for(j = 0;j < t->fpos[0]; j++)
					f[n].fp[j+1] = t->fpos[j+1];
				f[n].fp[0] = t->fpos[0];				
				n++;
			}
			else				
			{
				for(j = 0;j < t->fpos[0]; j++)
					f[dup].fp[f[dup].fp[0]+j+1] = t->fpos[j+1];
				f[dup].fp[0]+=t->fpos[0];						
			}	
			k++;
		}
	}//end of if closure
}//end of FOLLOWPOS

void display_followpos()
{
	int i,j;

	printf("\nSYM\tPOS-->   FOLLOWPOS");
	for(i=0;i<n;i++)
	{
		printf("\n%c\t%d-->   ",f[i].sym,f[i].pos);
		for(j=0;j<f[i].fp[0];j++)
			printf("%d ",f[i].fp[j+1]);
		
	}
}//end of display_followpos

void construct_DFA(T *t)
{
	ST   st[MAX]; 	
	int i, j, no_states, final, j1;
	
	final = t->r->fpos[1];
	initialise(st);
	//Construction of DFA
	printf("\n ---------\n");
	printf("\nStart state:");
	for(i=0;i<t->fpos[0];i++)
		printf("%d ",st[0].set[i+1]=t->fpos[i+1]);
		
	st[0].set[0]=t->fpos[0];

	no_states = 1;  //no of states	
	for(i=0;i<no_states;i++)    //states
	{
		for(j=0;j<st[i].set[0];j++) // elements in each state
		{
			for(j1=0;j1<n;j1++)//   follwowpos array
			{			
				if(st[i].set[j+1]==f[j1].pos&&f[j1].sym=='a')
				{
					union1(st[i].seta,f[j1].fp);
				}
				if(st[i].set[j+1]==f[j1].pos&&f[j1].sym=='b')
				{
					union1(st[i].setb,f[j1].fp);
				}
			}//for j1
		}//for j
		
		if(is_newstate(st,st[i].seta,no_states))
		{
			for(j1=0;j1<=st[i].seta[0];j1++)
				st[no_states].set[j1]=st[i].seta[j1];			
			no_states++;	
		}
		if(is_newstate(st,st[i].setb,no_states))
		{
			for(j1=0;j1<=st[i].setb[0];j1++)
				st[no_states].set[j1]=st[i].setb[j1];
									
			no_states++;	
		} 
	}//for i

	printf("\n Printing the Transition Table:\n");
	printf("\nSTATE\t\tA\t\tB ");
	for(i = 0;i < no_states;i++)
	{
		printf("\n");
		if(i == 0)
			printf("IS: ");			
		else if(check_final(st[i].set,final))
			printf("FS: ");		
		else
			printf("    ");
		
		for(j = 1;j <= st[i].set[0]; j++)
		{
			printf("%d,",st[i].set[j]);
		}		
		printf("\b ");
		printf("\t");	
			
		for(j = 1;j <= st[i].seta[0]; j++)
		{
			printf("%d,",st[i].seta[j]);
		}
		printf("\b ");
		printf("\t");			
		
		for(j = 1;j <= st[i].setb[0]; j++)
		{
			printf("%d,",st[i].setb[j]);
		}
		printf("\b ");
	}//for i
	printf("\n\n");
}

int main()
{
	int k,j,l,i=0,pos_count=0;
	S *top = NULL;
	T *t;
	char s[MAX],ch;	
		
	printf("\nEnter the postfix expression:");
	scanf("%s",s);
	printf("\nexpr=%s\n",s);

	while((ch=s[i++]))  //traversing  the string
	{
		t = getnode_t();
		t->c = ch;
		if(isalnum(ch)||ch == '#')
		{
			pos_count++;
			t->pos = pos_count;
			top = push(top,t);
			t->fpos[1] = t->lpos[1] = pos_count;
			t->fpos[0] = t->lpos[0] = 1;
		}
		else
		{
			if(t->c == '*')        //   for kleen closure
			{
				t->r = NULL;
				t->l = pop(&top);
				t->pos = -1;
				top = push(top,t);
			}
			else
			{
				t->r = pop(&top);
				t->l = pop(&top);
				t->pos = -1;
				top = push(top,t);
			}
		}
		NULLABLE(t);		
		FIRSTPOS(t);
		LASTPOS(t);
		FOLLOWPOS(t);
	}//WHILE
	
	t = pop(&top);	
	printf("\nDATA ( pos,nullable)\t\tFIRSTPOS\tLASTPOS");
	inorder(t); //Print First pos and Last pos
	printf("\n");

	inorder_s(t,n,f);
	display_followpos();
	printf("\n");
	construct_DFA(t);
	return 0;
}

