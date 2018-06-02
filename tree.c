// 2014B4A70530P
// GOVIND MITTAL

#include "common.h"
int treeSize = 0;
ParseTree addRule(ParseTree parent, struct elem* e)
{
	if(e == NULL)return parent;
	while (e)
	{
		ParseTree p = (ParseTree)malloc(sizeof(struct node));
		treeSize++;
		p->sibling = NULL;
		p->firstchild = NULL;
		p->parent = parent;
		p->token = (struct lexeme*)malloc(sizeof(struct lexeme));
		p->token->symbol = e->symbol;
		memset(p->token->info, 0, ID_LEN);
		p->token->line_no = 0;
		p->isLeaf = e->isTerminal;
		p->visited = 0;
		if(parent->firstchild == NULL)
			parent->firstchild = p;
		else
		{
			ParseTree curr = parent->firstchild;
			while(curr->sibling)
				curr = curr->sibling;
			curr->sibling = p;
		}
		e = e->next;
	}
	return parent->firstchild;
}
ParseTree addTokenInfo(ParseTree pt, struct lexeme* token)
{
	strcpy(pt->token->info, token->info);
	pt->token->line_no = token->line_no;
	while(pt->sibling == NULL && pt->parent)
		pt = pt->parent;
	return pt->sibling;
}

void visit_node(ParseTree pt)
{
	pt->visited = 1;
	int temp, i;
	char a_NodeSymbol[ID_LEN], a_line_no[ID_LEN], a_value_num[ID_LEN], a_token[ID_LEN],
		a_lexemeCurrentNode[ID_LEN], a_parentNodeSymbol[ID_LEN], a_isLeafNode[ID_LEN];
	char lexemeCurrentNode[ID_LEN];
	int line_no;
	char token[ID_LEN];
	char value_num[ID_LEN];
	float value_rnum;
	char parentNodeSymbol[ID_LEN];
	char isLeafNode[5];
	char NodeSymbol[ID_LEN];

	if(pt->isLeaf)
	{
		printSymbolName(pt->token->symbol, lexemeCurrentNode);
		strcpy(isLeafNode, "YES");
		strcpy(NodeSymbol, "----");
	}
	else
	{
		strcpy(lexemeCurrentNode, "----");
		strcpy(isLeafNode, "NO");
		printSymbolName(pt->token->symbol, NodeSymbol);
	}
	line_no = pt->token->line_no;
	if(pt->parent)
		printSymbolName(pt->parent->token->symbol, parentNodeSymbol);
	else strcpy(parentNodeSymbol, "ROOT");

	strcpy(a_lexemeCurrentNode, lexemeCurrentNode);
	strcat(a_lexemeCurrentNode, "\t");
	temp = strlen(lexemeCurrentNode);
	// if(temp < 4)
		// strcat(a_lexemeCurrentNode, "\t");
    if(temp < 8)
		strcat(a_lexemeCurrentNode, "\t");

	i = 0;
	do
	{
		temp = line_no%10;
		a_line_no[i++] = (char)(temp + '0');
		line_no /= 10;
	}while(line_no != 0);
	a_line_no[i] = '\0';
	strcat(a_line_no, "\t");

	sprintf(a_parentNodeSymbol,"%24s\t\t", parentNodeSymbol);

	strcpy(a_isLeafNode, isLeafNode);
	strcat(a_isLeafNode, "\t\t");

	strcpy(a_NodeSymbol, NodeSymbol);
	
	if(pt->token->symbol == TK_NUM)
	{
		strcpy(value_num, pt->token->info);
		strcpy(a_value_num, value_num);
		temp = strlen(value_num);
		temp = temp/4;
		for (; temp <= 4; temp++)
			strcat(a_value_num, "\t");
		strcpy(a_token, "----\t\t\t\t\t");
	}
	else if(pt->token->symbol == TK_RNUM)
	{
		value_rnum = atoi(pt->token->info)/100.0;
		strcpy(value_num, pt->token->info);
		sprintf(a_value_num,"%.2f", value_rnum);
		temp = strlen(value_num)+1;
		temp = temp/4;
		for (; temp <= 4; temp++)
			strcat(a_value_num, "\t");
		strcpy(a_token, "----\t\t\t\t\t");
	}
	else if(pt->token->info[0])
	{
		strcpy(token, pt->token->info);
		strcpy(a_token, token);
		temp = strlen(token);
		temp = temp/4;
		for (; temp <= 4; temp++)
			strcat(a_token, "\t");
		strcpy(a_value_num, "----\t\t\t\t\t");
	}
	else
	{ 
		strcpy(a_token, "----\t\t\t\t\t");
		strcpy(a_value_num, "----\t\t\t\t\t");
	}
	printf("%s%s%s%s%s%s%s\n",
		a_lexemeCurrentNode, a_line_no, a_token, a_value_num,
		 a_parentNodeSymbol, a_isLeafNode, a_NodeSymbol);
}
void inorder_traversal(ParseTree pt)
{	
	if(pt->token->symbol == EPSILON || pt->visited) 
		return;
	else visit_node(pt);
	if(pt->firstchild) 
		inorder_traversal(pt->firstchild);
	while(pt->sibling)
	{
		inorder_traversal(pt->sibling);
		pt = pt->sibling;
	}
}

int getTreeSize()
{
	return treeSize;
}