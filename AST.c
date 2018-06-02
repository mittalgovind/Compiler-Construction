// 2014B4A70530P
// GOVIND MITTAL
#include "common.h"
#define GLOBAL_OFFSET 0

int line_no = 1;
int astSize = 0;
SymbolTable* globalST;
SymbolTable* currST;
FunctionTable* globalFT;
FunctionTable* currFT;
FunctionTableElement* currFTE;
int currOffset = GLOBAL_OFFSET;
int used;
char* currentLHSname;
char typename[1024];

char* typeOfID(Node* ast, char* type)
{ 
	SymbolTableElement* e = findSymbol(currST, ast->info);
	char name[50];
	if(!e)
		printf("Line %d: Variable %s is not declared.\n", line_no, ast->info);
	type = printSymbolName(e->type, type);
	if(e->type == TK_MATRIX)
	{
		strcat(type, "<");
		sprintf(name, "%d", e->matrows);
		strcat(type, name);
		strcat(type, ",");
		sprintf(name, "%d", e->matcols);
		strcat(type, name);
		strcat(type, ">");
	}
	return type;
}

char* typeOfVarList(Node* ast, char* type)
{
	while(ast)
	{
		strcat(type, typeOfID(ast->child, typename));
		// strcat(type, ",");
		ast = ast->next;
	}
	return type;
}

char* typeOfMat(Node* ast, char* type)
{
	Node* temp = ast->child;
	int matrows = 0;
	int matcols = 0;
	while(temp && temp->child)
	{
		if(strncmp(typeOfLHS(temp->child, type), "INT", 3) != 0)
			printf("Line %d: Only integer type allowed in matrix\n.", line_no);
		matrows++;
		temp = temp->next;
	}
	temp = ast;
	while(temp)
	{
		matcols++;
		temp = temp->next;
	}
	sprintf(type, "MATRIX<%d,%d>", matrows, matcols);
	return type;
}

char totaltype[1024];
int flag;
void parseAST(Node* ast)
{
	if(flag == 0 && ast && (ast->symbol == TK_NUM || ast->symbol == TK_STR
		|| ast->symbol == TK_RNUM || ast->symbol == TK_ID || ast->symbol == 230 ))
		strcpy(totaltype, typeOfLHS(ast, typename));
	else if(flag == 1 && ast && (ast->symbol == TK_NUM || ast->symbol == TK_STR
		|| ast->symbol == TK_RNUM || ast->symbol == TK_ID || ast->symbol == 230 ) && strcmp(totaltype, typeOfLHS(ast, typename)) != 0)
	{
		printf("Line %d: Type Mismatch in the arithmetic expression", line_no);
	}
	if(ast->child)
		parseAST(ast->child);
	if(ast->next)
		parseAST(ast->next);
}
char* typeOfArithmeticExpr(Node* ast, char* type)
{
	if(ast->child->child->symbol == 230) return typeOfMat(ast->child->child, type);
	strcpy(type, typeOfLHS(ast->child->child, type));
	memset(totaltype, '\0', 1024);
	flag = 0;
	parseAST(ast);
}

char* typeOfFunctionCall(Node* ast, char* type)
{
	return NULL;
}

char* typeOfSizeOf(Node* ast, char* type)
{
	if(findSymbol(currST, ast->next->info)->type == TK_MATRIX)
		return "INTINT";
	SymbolTableElement* e = findSymbol(currST, ast->next->child->info);
	return printSymbolName(e->type, type);	
}

void calculateMatSize(Node* ast)
{
	SymbolTableElement* elem = findSymbol(currST, currentLHSname);
	Node* temp = ast->child;
	elem->matrows = 0;
	elem->matcols = 0;
	while(temp && temp->child)
	{
		if(strncmp(typeOfLHS(temp->child, typename), "INT", 3) != 0)
			printf("Line %d: Only integer type allowed in matrix.\n", line_no);
		elem->matrows++;
		temp = temp->next;
	}
	temp = ast;
	while(temp)
	{
		elem->matcols++;
		temp = temp->next;
	}
}

char* typeOfLHS(Node* ast, char* type)
{
	char* name = "";
	memset(type, '\0', 1024);
	switch(ast->symbol)
	{
		case TK_ID	: return typeOfID(ast, type);
		case TK_NUM : return "INT";
		case TK_RNUM: return "REAL";
		case TK_STR : return "STRING";
		case 230	: return typeOfMat(ast, type);
		case 235	: return typeOfVarList(ast, type);
		default: return NULL;
	}
	return NULL;
}

char* typeOfRHS(Node* ast, char* type)
{
	switch(ast->symbol)
	{
		// arithmeticExpr
		case 220:  return typeOfArithmeticExpr(ast, type);
		// functionCall
		case 214:  return typeOfFunctionCall(ast, type);
		// sizeOf
		case 215:  return typeOfSizeOf(ast, type);
		default: strcpy(type, "empty");
	}
	return type;
}

Node* mkleaf(ParseTree pt)
{
	Node *ast = (Node*)malloc(sizeof(Node));
	astSize++;
	ast->symbol = pt->token->symbol;
	ast->line_no = pt->token->line_no;
	memset(ast->info,'\0',ID_LEN);
	strcpy(ast->info, pt->token->info);
	ast->child = NULL;
	ast->next = NULL;
	return ast;
}

Node* mknode(ParseTree pt, Node* c0, Node* c1, Node* c2, Node* c3)
{
	Node *ast = mkleaf(pt);
	Node* temp = ast;

	if(c0) temp->child = c0;
	else temp->child = NULL;

	if(c1) 
	{
		temp->next = c1;
		temp = temp->next;
	}
	else return ast;

	if(c2) 
	{
		temp->next = c2;
		temp = temp->next;
	}
	else return ast;

	if(c3) 
	{
		temp->next = c3;
		temp = temp->next;
	}

	return ast;
}

Node* reduceMainFunction(ParseTree root)
{
	globalST = createSymbolTable(NULL, "main");
	currST = globalST;
	globalFT = createFunctionTable(NULL);
	currFTE = insertFunction(NULL, globalFT, "main", 0, 0, NULL, NULL, NULL, NULL, currST);
	currFT = currFTE->FT;
	Node *ast = reduceStmtAndFunctionDefs(
					root->firstchild->sibling->sibling->sibling);
	currFT = currFTE->parent;
	return ast;
}

Node* reduceStmtAndFunctionDefs(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node* c1 = reduceStmtOrFunctionDef(root->firstchild);
	Node* c2 = reduceMoreStatements(root->firstchild->sibling);
	Node *ast = mknode(root, c1, c2, NULL, NULL);
	return ast;
}

Node* reduceStmtOrFunctionDef(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	line_no++;
	switch(root->firstchild->token->symbol)
	{
		case 204:
			ast = reduceStmt(root->firstchild);
			break;
		case 205: 
			ast = reduceFunctionDef(root->firstchild);
			break;
		default: printf("Invalid Stmt or functionDef\n");
	}
	return ast;
}

Node* reduceStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	switch(root->firstchild->token->symbol)
	{
		case 209: 
			ast = reduceDeclarationStmt(root->firstchild); 
			break;

		case 210: 
			ast = reduceAssignmentStmt(root->firstchild); 
			break;

		case 211: 
			ast = reduceFunCallStmt(root->firstchild); 
			break;

		case 216: 
			ast = reduceConditionalStmt(root->firstchild); 
			break;

		case 219: 
			ast = reduceIoStmt(root->firstchild); 
			break;
		default: 
			printf("Invalid Statement\n");
			line_no--;

	}
	return ast;
}

Node* reduceFunctionDef(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1, *c2, *c3, *temp;
	FunctionTableElement* check;
	c1 = mkleaf(root->firstchild->sibling->sibling->sibling->sibling->sibling);
	currST = createSymbolTable(currST, c1->info);
	used = 0;
	c0 = reduceParameter_list(root->firstchild->sibling->sibling); 
	used = 1;
	c2 = reduceParameter_list(root->firstchild->sibling->sibling->sibling->sibling->sibling->sibling->sibling); 

	int	no_of_inputs = 0, no_of_outputs = 0, i;
	int *input_types, *output_types;
	char **input_ids, **output_ids;
	temp = c2;
	while(temp) 
	{
		no_of_inputs++;
		temp = temp->next;
	}
	temp = c0;
	while(temp) 
	{
		no_of_outputs++;
		temp = temp->next;
	}
	no_of_inputs /= 2; 
	no_of_outputs /= 2;
	input_types = (int*)malloc(sizeof(int)*no_of_inputs);
	output_types = (int*)malloc(sizeof(int)*no_of_outputs);
	input_ids = (char**)malloc(sizeof(char*)*no_of_inputs);
	output_ids = (char**)malloc(sizeof(char*)*no_of_outputs);
	temp = c2;
	for(i = 0; i < no_of_inputs; i++, temp=temp->next->next)
	{
		input_types[i] = temp->child->symbol;
		input_ids[i] = (char*)malloc(ID_LEN*sizeof(char));
		strcpy(input_ids[i], temp->next->info);
	}
	temp = c0;
	for(i = 0; i < no_of_outputs; i++, temp=temp->next->next)
	{
		output_types[i] = temp->child->symbol;
		output_ids[i] = (char*)malloc(ID_LEN*sizeof(char));
		strcpy(output_ids[i], temp->next->info);
	}

	check = insertFunction(currFTE, currFT, c1->info, no_of_inputs,
				 no_of_outputs, input_types, output_types, 
				 input_ids, output_ids, currST);
	if(!check)
		 return NULL;
	currFTE = check;
	currFT = currFTE->FT;
	c3 = reduceStmtAndFunctionDefs(root->firstchild->sibling->sibling->
			sibling->sibling->sibling->sibling->sibling->sibling->sibling);
	// deleteST(currST)
	for(i = 0; i < (int)pow(2, currST->t); i++)
		if ((currST->elements[i] != NULL) && (currST->elements[i]->used == 0))
			printf("Line %d: Output Variable %s has not been assigned a value.\n", line_no, currST->elements[i]->info);;
	
	printSymbolTable(currST);
	currFTE = currFT->parent;
	currFT = currFTE->FT;
	currST = currST->parent;
	ast = mknode(root, c0, c1, c2, c3);
	line_no++;
	return ast;
}	

Node* reduceParameter_list(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1, *c2;
	c0 = reduceType(root->firstchild);
	c1 = mkleaf(root->firstchild->sibling);
	int width;
	width = insertSymbol(currST, c0->symbol, currOffset, c1->info, used);
	currOffset += width;

	c2 = reduceRemainingList(root->firstchild->sibling->sibling);
	ast = mknode(root, c0, c1, c2, NULL);
	return ast;
}

Node* reduceType(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ast = mkleaf(root->firstchild);
	return ast;
}

Node* reduceRemainingList(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast;
	ast = reduceParameter_list(root->firstchild->sibling);
	return ast;
}

Node* reduceDeclarationStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1;
	c0 = reduceType(root->firstchild);
	c1 = reduceVar_list(root->firstchild->sibling);
	ast = mknode(root, c0, c1, NULL, NULL);

	Node* temp = ast->next;
	int width;
	while(temp)
	{
		width = insertSymbol(currST, ast->child->symbol, currOffset, temp->child->info, 1);
		currOffset += width;
		temp = temp->next;
	}

	return ast;
}

Node* reduceAssignmentStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1, *c2;
	char type1[1024];
	memset(type1, '\0', 1024);
	char type2[1024];
	memset(type2, '\0', 1024);
	c0 = mkleaf(root->firstchild->sibling);
	c1 = reduceLeftHandSide(root->firstchild);
	c2 = reduceRightHandSide(root->firstchild->sibling->sibling);
	strcpy(type1, typeOfLHS(c1, typename));
	strcpy(type2, typeOfRHS(c2, typename));
	if(strcmp(type1, type2) != 0)
		printf("Line %d: Type Mismatch. LHS = %s, RHS = %s.\n", line_no, 
				type1, type2);
	ast = mknode(root, c0, c1, c2, NULL);
	return ast;
}

Node* reduceFunCallStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ast = reduceFunctionCall(root->firstchild);
	char type1[1024];
	strcpy(type1, typeOfRHS(ast, typename));
	if(strcmp(type1, "NULL")) 
		printf("Line %d: Type Mismatch. Return type of function should be empty but is %s.\n", line_no, 
					type1);
	return ast;
}

Node* reduceLeftHandSide(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	SymbolTableElement* temp;
	switch(root->firstchild->token->symbol)
	{
		case TK_ID: 
			ast = mkleaf(root->firstchild); 
			temp = findSymbol(currST, ast->info);
			currentLHSname = ast->info;
			if(temp) temp->used = 1;
			else 
				printf("Line %d: Variable %s not found.\n", line_no, ast->info);
			break;

		case TK_SQO:
			ast = reduceVar_list(root->firstchild->sibling);
			break;
		default: printf("Invalid Left Hand Side\n");
	}

	return ast;
}

Node* reduceRightHandSide(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	switch(root->firstchild->token->symbol)
	{
		case 214: 
			ast = reduceFunctionCall(root->firstchild);
			break;
		case 215: 
			ast = reduceSizeOf(root->firstchild);
			break;
		case 220: 
			ast = reduceArithmeticExpression(root->firstchild);
			break;
		default: printf("Invalid Right Hand Side\n");
	}
	return ast;
}

Node* reduceFunctionCall(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1;
	c0 = mkleaf(root->firstchild);
	if(strcmp(c0->info, currST->scope) == 0)
	{
		printf("Line %d: Recursion of %s is not allowed\n", line_no, c0->info);
		return NULL;
	}
	c1 = reduceInput_list(root->firstchild->sibling->sibling);
	ast = mknode(root, c0, c1, NULL, NULL);
	return ast;
}

Node* reduceSizeOf(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1;
	c0 = mkleaf(root->firstchild);
	c1 = mkleaf(root->firstchild->sibling);
	ast = mknode(root, c0, c1, NULL, NULL);
	findSymbol(currST, c1->info);
	return ast;
}

Node* reduceConditionalStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1, *c2;
	c0 = reduceBooleanExpression(root->firstchild->sibling->sibling);
	c1 = reduceOtherStmts(root->firstchild->sibling->sibling->sibling->sibling);
	c2 = reduceElse_stmt(root->firstchild->sibling->sibling->sibling->sibling->sibling);
	ast = mknode(root, c0, c1, c2, NULL);

	return ast;
}

Node* reduceElse_stmt(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast;
	ast = reduceOtherStmts(root->firstchild->sibling);
	return ast;
}

Node* reduceOtherStmts(ParseTree root)
{
	return reduceStmtAndFunctionDefs(root->firstchild);
}

Node* reduceIoStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1;
	c0 = mkleaf(root->firstchild);
	c1 = mkleaf(root->firstchild->sibling->sibling);
	if(c1->symbol != TK_ID)
	{
		printf("Line %d: Only variable names allowed in IO-stmt\n", line_no);
		return NULL;
	}
	ast = mknode(root, c0, c1, NULL, NULL);
	if (!findSymbol(currST, ast->next->info))
		printf("Line %d: Variable %s is not declared.\n", line_no, ast->next->info);
	return ast;
}

Node* reduceArithmeticExpression(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1, *c2;
	switch(root->firstchild->token->symbol)
	{
		case 115:
			c0 = reduceArith_operator(root->firstchild->sibling->sibling->sibling);
			c1 = reduceArithmeticExpression(root->firstchild->sibling);
			c2 = reduceArithmeticExpression(root->firstchild->sibling->sibling->sibling->sibling->sibling);
			ast = mknode(root, c0, c1, c2, NULL);
		 	break;

		case 222:
			c0 = reduceSub_expression(root->firstchild);
			c1 = reduceTerm(root->firstchild->sibling);
			if(c1 == NULL && c0->child->symbol == 230)
				calculateMatSize(c0->child);
			ast = mknode(root, c0, c1, NULL, NULL);
			break;
		default: printf("Invalid Arithmetic Expr\n");
	}
	return ast;
}

Node* reduceTerm(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast, *c0, *c1, *c2;
	c0 = reduceOperator1(root->firstchild);
	c1 = reduceSub_expression(root->firstchild->sibling);
	c2 = reduceTerm(root->firstchild->sibling->sibling);
	ast = mknode(root, c0, c1, c2, NULL);
	return ast;
}

Node* reduceSub_expression(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	Node *ast, *c0, *c1;
	c0 = reduceVar(root->firstchild);
	c1 = reduceFactor(root->firstchild->sibling);
	ast = mknode(root, c0, c1, NULL, NULL);
	return ast;
}

Node* reduceFactor(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast, *c0, *c1, *c2;
	c0 = reduceOperator2(root->firstchild);
	c1 = reduceVar(root->firstchild->sibling);
	c2 = reduceFactor(root->firstchild->sibling->sibling);
	ast = mknode(root, c0, c1, c2, NULL);
	return ast;
}

Node* reduceOperator1(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ast = mkleaf(root->firstchild);
	return ast;
}

Node* reduceOperator2(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ast = mkleaf(root->firstchild);
	return ast;
}

Node* reduceArith_operator(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ast = mkleaf(root->firstchild);
	return ast;
}

Node* reduceBooleanExpression(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1, *c2;
	switch(root->firstchild->token->symbol)
	{
		case 115:
			c0 = reduceLogicalOp(root->firstchild->sibling->sibling->sibling);
			c1 = reduceBooleanExpression(root->firstchild->sibling);
			c2 = reduceBooleanExpression(root->firstchild->sibling->sibling->sibling->sibling->sibling);
			ast = mknode(root, c0, c1, c2, NULL);
		 	break;

		case 228:
			c0 = reduceRelationalOp(root->firstchild->sibling);
			c1 = reduceVar(root->firstchild);
			c2 = reduceVar(root->firstchild->sibling->sibling);
			if(((strncmp(typeOfLHS(c1->child, typename), "INT", 3) == 0  && strncmp(typeOfLHS(c2->child, typename), "INT", 3) == 0 )||
				(strncmp(typeOfLHS(c1->child, typename), "REAL", 4) == 0  && strncmp(typeOfLHS(c2->child, typename), "REAL", 4) == 0)) == 0)
				printf("Line %d: Only integer or real types allowed in boolean expressions.", line_no);
			ast = mknode(root, c0, c1, c2, NULL);
			break;

		case 132:
			c0 = mkleaf(root->firstchild);
			c1 = reduceBooleanExpression(root->firstchild->sibling->sibling);
			ast = mknode(root, c0, c1, NULL, NULL);
		 	break;

		default: printf("Invalid Arithmetic Expr\n");
	}
	return ast;
}

Node* reduceVar(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ParseTree temp;
	if(root->firstchild->token->symbol == 229)
		ast = reduceMatrixElement(root->firstchild);
	else ast = mkleaf(root->firstchild);
	SymbolTableElement* found = findSymbol(currST, ast->info);

	if(root->firstchild->token->symbol == TK_ID)
	{
		if((root->parent->token->symbol == 222 || 
			root->parent->token->symbol == 223 || 
			root->parent->token->symbol == 227 ) && !found)
			printf("Line %d: Variable %s is not declared.\n", line_no, ast->info);
		else if(root->parent->token->symbol == 235)
		{
			temp = root->parent->parent;
			while(temp->token->symbol != 212 && temp->token->symbol != 239 && temp->token->symbol != 209)
				temp = temp->parent;
			if((temp->token->symbol == 212 || temp->token->symbol == 239 ) && !found)
				printf("Line %d: Variable %s is not declared.\n", line_no, ast->info);

			else if(temp->token->symbol == 209 && found)
				printf("Line %d: Variable %s is already declared in scope of %s\n", line_no, ast->info, currST->scope);			
		}
	}
	return ast;
}

Node* reduceMatrixElement(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ast = reduceMatrixValues(root->firstchild->sibling);
	return ast;
}

Node* reduceMatrixValues(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1;
	c0 = reduceRow_list(root->firstchild);
	c1 = reduceRem_rowlist(root->firstchild->sibling);
	ast = mknode(root, c0, c1, NULL, NULL);
	return ast;
}

Node* reduceRow_list(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1;
	c0 = mkleaf(root->firstchild);
	c1 = reduceRest_list(root->firstchild->sibling);
	ast = mknode(root, c0, c1, NULL, NULL);
	return ast;
}

Node* reduceRest_list(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast;
	ast = reduceRow_list(root->firstchild->sibling);
	return ast;
}

Node* reduceLogicalOp(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ast = mkleaf(root->firstchild);
	return ast;
}

Node* reduceRelationalOp(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast;
	ast = mkleaf(root->firstchild);
	return ast;
}

Node* reduceVar_list(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	Node *ast, *c0, *c1;
	c0 = reduceVar(root->firstchild);
	c1 = reduceMore_ids(root->firstchild->sibling);
	ast = mknode(root, c0, c1, NULL, NULL);
	return ast;
}

Node* reduceMore_ids(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast;
	ast = reduceVar_list(root->firstchild->sibling);
	return ast;
}

Node* reduceMoreStatements(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast;
	ast = reduceStmtAndFunctionDefs(root->firstchild);
	return ast;
}

Node* reduceRem_rowlist(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast;
	ast = reduceMatrixValues(root->firstchild->sibling);
	return ast;
}

Node* reduceInput_list(ParseTree root)
{
	if(root == NULL || root->firstchild && root->firstchild->token->symbol == EPSILON)
		return NULL;
	Node *ast;
	ast = reduceVar_list(root->firstchild);
	return ast;
}

int getAstSize()
{
	return astSize;
}

void printAST(Node* ast)
{
	if(ast)
		printf("%s \t\t\t%s\n", printSymbolName(ast->symbol, typename), ast->info);
	if(ast->child)
		printAST(ast->child);
	if(ast->next)
		printAST(ast->next);
}