// 2014B4A70530P
// GOVIND MITTAL
#include "common.h"

int cor[][2] = {
				 {236, 235},
				 {239, 235},
				 {237, 202},
				 {208, 206},
				 {231, 230},
				 {232, 231},
				 {238, 230},
				 {236, 239},
				 {239, 236},
				 {202, 218}
				};

// set operations
set powersof2[64];
set unions(set a, int b){return a | powersof2[b];}
set unionsets(set a, set b){return a | b;}
set intersect(set a, int b){return a & powersof2[b];}
set contains(set s, int k){return s & powersof2[k];}
set setremove(set s, int k){return s & ~powersof2[k];}

void calcPowers()
{
	int i;
	powersof2[0] = 1;
	for(i = 1; i < 64; i++ )
		powersof2[i] = powersof2[i-1] * 2;
}

// functions for generating parse table
struct elem* createRhs(unsigned int symbol)
{	
	struct elem* temp = (struct elem*)malloc(sizeof(struct elem));
	temp->symbol = symbol;
	if (symbol >= NON_TERMINALS_BASE)
		temp->isTerminal = FALSE;
	else temp->isTerminal = TRUE;
	temp->next = NULL;
	return temp;
}

struct elem* populateRule(char* right)
{
	char *k;
	k = strtok(right,",");
	struct elem* p = createRhs(atoi(k));
	struct elem* temp = p;
	k = strtok(NULL, ",");
	while(k)
	{
		temp->next = createRhs(atoi(k));
		temp = temp->next;
		k = strtok(NULL, ",");
	}
	return p;
}
	
Rule createRule(char* right)
{
	Rule new_rule = (Rule)malloc(sizeof(struct rule));
	new_rule->subfirstset = 0;
	new_rule->subterminals = 0;
	new_rule->subfollowset = 0;	
	new_rule->next = NULL;
	new_rule->rhs = populateRule(right);
	return new_rule;
}

void createLhs(int symbol, char* right, LHS* G[NON_TERMINALS])
{
	G[symbol - NON_TERMINALS_BASE] = (LHS*)malloc(sizeof(LHS));
	G[symbol - NON_TERMINALS_BASE]->symbol = symbol;
	G[symbol - NON_TERMINALS_BASE]->info = createRule(right);
	G[symbol - NON_TERMINALS_BASE]->firstset = 0;
	G[symbol - NON_TERMINALS_BASE]->followset = 0;
}

void addToLhs(int symbol, char* right, LHS* G[NON_TERMINALS])
{
	Rule curr = G[symbol - NON_TERMINALS_BASE]->info;
	while(curr->next != NULL) 
		curr = curr->next;
	curr->next = createRule(right);
}

void populateGrammar(FILE* fp, LHS* G[NON_TERMINALS])
{
	char temp[RULE_LEN];
	int i;
	int start;
	for(i = 0; i < RULES; i++){
		fscanf(fp, "%d=%s\n",&start, temp);
		if(G[start - NON_TERMINALS_BASE] == NULL || 
		   G[start - NON_TERMINALS_BASE]->info == NULL)
			createLhs(start, temp, G);
		else addToLhs(start, temp, G);
	}
}

void computeFirstandFollowSets(LHS* G[NON_TERMINALS])
{
	int i, j;

	// generate first sets
	int processed[NON_TERMINALS];
	unsigned long all_processed = 0;
	for (int i = 0; i < NON_TERMINALS; ++i)
		processed[i] = 1;		

	for(i = 0; i < NON_TERMINALS; i++)
		for (Rule info = G[i]->info; info; info = info->next)
				processed[i] *= info->rhs->isTerminal;

	for(i = 0; i < NON_TERMINALS; i++)
		for (Rule info = G[i]->info; info; info = info->next)
			if(info->rhs->isTerminal)
				info->subfirstset = unions(info->subfirstset,
					info->rhs->symbol - TERMINALS_BASE);
			else
				info->subterminals = unions(info->subterminals,
							info->rhs->symbol - NON_TERMINALS_BASE);
	
	for (i = 0; i < NON_TERMINALS; i++)
		for (Rule info = G[i]->info; info; info = info->next)
			G[i]->firstset = unionsets(G[i]->firstset, info->subfirstset);

	while(!all_processed)
	{
		for (i = 0; i < NON_TERMINALS; i++)
		{
			if(processed[i])
				for (j = 0; j < NON_TERMINALS; j++)
				{
					if(processed[j])
						continue;
					for (Rule info = G[j]->info; info; info = info->next)
						if(contains(info->subterminals, 
									G[i]->symbol - NON_TERMINALS_BASE))
						{
							info->subterminals = setremove(info->subterminals,
										 G[i]->symbol - NON_TERMINALS_BASE);
							info->subfirstset = unionsets(info->subfirstset,
															 G[i]->firstset);
							processed[j] = 1;
							for (Rule r = G[j]->info; r; r = r->next)
									processed[j] *= r->subfirstset ? 1 : 0 ;
						}
					if(processed[j])
						for (Rule info = G[j]->info; info; info = info->next)
							G[j]->firstset = unionsets(G[j]->firstset,
														 info->subfirstset);
				}
		}
		all_processed = 1;
		for (i = 0; i < NON_TERMINALS; i++)
			all_processed *= processed[i];
	}

	// generate follow sets
	for (i = 0; i < NON_TERMINALS; i++)
	{
		for (j = 0; j < NON_TERMINALS; j++)
			for (Rule info = G[j]->info; info; info = info->next)
				for(struct elem* e = info->rhs; e; e = e->next)
					if(e->symbol == G[i]->symbol)
					{					
						if(e->next == NULL)
							G[i]->followset = unionsets(G[i]->followset,
														 G[j]->followset);
						else 
						{
							if(e->next->isTerminal)
								G[i]->followset = unions(G[i]->followset,
										 e->next->symbol - TERMINALS_BASE);
							else 
							{
								G[i]->followset = unionsets(G[i]->followset,
											setremove(G[j]->firstset, 
												EPSILON - TERMINALS_BASE));
								if(contains(G[j]->firstset,
												 EPSILON - TERMINALS_BASE))
									G[i]->followset = unionsets(G[i]->followset,
														 G[j]->followset);
							}
						}
						break;
					}
	}
	G[17]->followset = 0b1100000000000000000000; 
	for (i = 0; i < 10; i++)
		G[cor[i][0]-NON_TERMINALS_BASE]->followset = unionsets(
							G[cor[i][0]-NON_TERMINALS_BASE]->followset, 
							G[cor[i][1]-NON_TERMINALS_BASE]->followset);
	G[30]->followset = unionsets(G[30]->followset, G[37]->firstset);
	for (i = 0; i < 10; i++)
		G[cor[i][0]-NON_TERMINALS_BASE]->followset = unionsets(
							G[cor[i][0]-NON_TERMINALS_BASE]->followset, 
							G[cor[i][1]-NON_TERMINALS_BASE]->followset);
}

void createParseTable(LHS* G[NON_TERMINALS], Table T)
{
	int i, j;
	for(i = 0; i < NON_TERMINALS; i++)
	{
		for(j = 1; j < TERMINALS; j++)
		{
			Rule info;
			for (info = G[i]->info; info; info = info->next)
				if(contains(info->subfirstset, j) ||
					(contains(info->subfirstset, EPSILON - TERMINALS_BASE) &&
					  contains(G[i]->followset, j)))
					break;	
			T[i][j] = info;
		}
	}
}

set createSyncSet(set followset)
{
	set syncset = 0;
	syncset = unions(syncset, TK_SEMICOLON);
	syncset = unions(syncset, TK_INT);
	syncset = unions(syncset, TK_REAL);
	syncset = unions(syncset, TK_STRING);
	syncset = unions(syncset, TK_MATRIX);
	syncset = unions(syncset, TK_IF);
	syncset = unions(syncset, TK_ELSE);
	syncset = unions(syncset, TK_ENDIF);
	syncset = unions(syncset, TK_READ);
	syncset = unions(syncset, TK_PRINT);
	syncset = unions(syncset, TK_FUNCTION);
	syncset = unions(syncset, TK_END);
	syncset = unionsets(syncset, followset);
	return syncset;
}

ParseTree parseInputSourceCode(const char *testCaseFile, Table T, LHS* G[NON_TERMINALS])
{
	int currSymbol;
	int matched;
	char temp[ID_LEN];
	int isError = 0;
	struct lexeme* token = (struct lexeme*)malloc(sizeof(struct lexeme));
    FileBuffer* filebuffer = (FileBuffer*)malloc(sizeof(FileBuffer));

    init_lexer(testCaseFile, token, filebuffer);

	Stack* S = (Stack*)malloc(sizeof(Stack));
	S->head = NULL;
	S->size = 0;

	// pushing the start symbol to the top of the stack
	struct elem* start = (struct elem*)malloc(sizeof(struct elem));
	start->symbol = NON_TERMINALS_BASE;
	start->isTerminal = 0;
	start->next = NULL;
	push(S, start);

	// initialising the parse tree with the start symbol
	ParseTree pt = (ParseTree)malloc(sizeof(struct node));
	pt->isLeaf = 0;
	pt->visited = 0;
	pt->sibling = pt->firstchild = pt->parent = NULL;
	pt->token = (struct lexeme*)malloc(sizeof(struct lexeme));
	pt->token->symbol = NON_TERMINALS_BASE;
	memset(pt->token->info, 0, ID_LEN);
	pt->token->line_no = 0;

	set syncset;

	ParseTree currNode = pt;
	do
	{
		token = getNextToken(filebuffer, token);
		matched = 0;
		while(!matched && S->head != NULL)
		{
			currSymbol = S->head->e->symbol - NON_TERMINALS_BASE;
			// if non-terminal
			if(currSymbol >= 0)
			{
				if(T[currSymbol][token->symbol - TERMINALS_BASE])
				{
					pop(S);
					// stack
					pushRule(S, 
						T[currSymbol][token->symbol - TERMINALS_BASE]->rhs);
					// parsetree
					currNode = addRule(currNode, 
						T[currSymbol][token->symbol - TERMINALS_BASE]->rhs);
				}
				else 
				{
					isError = 1;
					printf("line %d: Syntax Error: The token %s \
					 for lexeme value = %s does not match at line number %d\n",
					  token->line_no, printSymbolName(token->symbol, temp),
					   token->info, token->line_no);
					syncset = createSyncSet(G[currSymbol]->followset);
					do
					{
						token = getNextToken(filebuffer, token);
					}while(!contains(syncset, token->symbol - TERMINALS_BASE)
							 && token->symbol != EOF);
					pop(S);
				}
			}
			else
			{
				currSymbol += NON_TERMINALS_BASE;
				if(currSymbol == token->symbol)
				{
					pop(S);
					currNode = addTokenInfo(currNode, token);
					matched = 1;
				}
				else 
				{
					isError = 1;
					printf("line %d: Syntax Error: The token %s \
					 for lexeme value = %s does not match stack top = %s at line number %d\n",
					  token->line_no, printSymbolName(token->symbol, temp),
					   token->info, printSymbolName(S->head->e->symbol, temp), token->line_no);
						pop(S);
				}
			}
			
			if(currNode && currNode->token && currNode->token->symbol == EPSILON)
			{
				while(currNode->sibling == NULL && currNode->parent)
					currNode = currNode->parent;
				currNode = currNode->sibling;
			}
		}
	} while(token->symbol != EOF);

	if(!isError) printf("Successfully Parsed!\n");
	fclose(filebuffer->fp);
	return pt;
}

ParseTree parse(const char *testCaseFile)
{
	int i;
	FILE* fp = fopen("numbered_grammar.txt", "r");
	LHS** G = (LHS**)malloc(sizeof(LHS*) * NON_TERMINALS);
	Table T = (Table)malloc(sizeof(Rule*) * NON_TERMINALS);
	for(i = 0; i < NON_TERMINALS; i++)
		T[i] = (Rule*)malloc(sizeof(Rule)* (TERMINALS+1));
	ParseTree pt;
	populateGrammar(fp, G);
	calcPowers();
	computeFirstandFollowSets(G);
	createParseTable(G, T);
	pt = parseInputSourceCode(testCaseFile, T, G);
	fclose(fp);
	for(i = 0; i < NON_TERMINALS; i++)
	{
		free(T[i]);
		free(G[i]->info->next);
		free(G[i]->info);
		free(G[i]);
	}
	free(G);
	free(T);
	return pt;
}
