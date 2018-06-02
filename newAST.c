#include "common.h"

ASTNode* ast;
ASTNode* createASTNode(ASTNode* pt)
{
	ASTNode* newast;
	newast = (ASTNode*)malloc(sizeof(ASTNode));
	newast->node = pt;
	newast->next = NULL; 	
	return newast;
}
ASTNode* reduceProgram(ParseTree root)
{	
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	return reduceMainFunction(root);
}

ASTNode* reduceMainFunction(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = createASTNode(reduceStmtAndFunctionDefs(
					root->firstchild->sibling->sibling->sibling));
	if(temp)
	{
	}
	free(root->firstchild->sibling->sibling->sibling->sibling);
	free(root->firstchild->sibling->sibling);
	free(root->firstchild->sibling);
	free(root->firstchild);
	free(root);

	return ast;
}

ASTNode* reduceStmtAndFunctionDefs(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;

	ast = createASTNode(reduceStmtOrFunctionDef(root->firstchild));
	if(temp)
	{
		ast = (ASTNode*)malloc(sizeof(ASTNode));
		ast->node = temp;
	}

	ast = createASTNode(reduceMoreStatements(root->firstchild->sibling));
	if(temp)
	{
		ast->next = (ASTNode*)malloc(sizeof(ASTNode));
		ast->next->node = temp;
		ast->next->next = NULL;
	}

	free(root);
	return ast;
}

ASTNode* reduceStmtOrFunctionDef(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;

	switch(root->firstchild->token->symbol)
	{
		case 204:
			ast = createASTNode(reduceStmt(root->firstchild));
			if(temp)
			{
				ast = (ASTNode*)malloc(sizeof(ASTNode));
				ast->node = temp;
				ast->next = NULL;
			}
			break;
		case 205: 
			ast = createASTNode(reduceFunctionDef(root->firstchild));
			if(temp)
			{
				ast = (ASTNode*)malloc(sizeof(ASTNode));
				ast->node = temp;
				ast->next = NULL;
			}
		default: printf("Invalid Stmt or functionDef");
	}
	free(root);
	return ast;
}

ASTNode* reduceStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	
	switch(root->firstchild->token->symbol)
	{
		case 209: 
			ast = createASTNode(reduceDeclarationStmt(root->firstchild));
			if(temp)
			{
				ast = (ASTNode*)malloc(sizeof(ASTNode));
				ast->node = temp;
				ast->next = NULL;
			} 
			break;

		case 210: 
			ast = createASTNode(reduceArithmeticExpression(root->firstchild));
			if(temp)
			{
				ast = (ASTNode*)malloc(sizeof(ASTNode));
				ast->node = temp;
				ast->next = NULL;
			} 
			break;

		case 211: 
			ast = createASTNode(reduceFunCallStmt(root->firstchild));
			if(temp)
			{
				ast = (ASTNode*)malloc(sizeof(ASTNode));
				ast->node = temp;
				ast->next = NULL;
			} 
			break;

		case 216: 
			ast = createASTNode(reduceConditionalStmt(root->firstchild));
			if(temp)
			{
				ast = (ASTNode*)malloc(sizeof(ASTNode));
				ast->node = temp;
				ast->next = NULL;
			} 
			break;

		case 219: 
			ast = createASTNode(reduceIoStmt(root->firstchild));
			if(temp)
			{
				ast = (ASTNode*)malloc(sizeof(ASTNode));
				ast->node = temp;
				ast->next = NULL;
			} 
			break;
		default: printf("Invalid Statement\n");
	}
	free(root);
	return ast;
}

ASTNode* reduceFunctionDef(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast = createASTNode(reduceParameter_list(root->firstchild->sibling->sibling));
	if(temp)
	ast->node 
	ast->next = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next->node = root->firstchild->sibling->sibling->sibling->sibling->sibling;
	ast = createASTNode(reduceParameter_list(ast->sibling->sibling->sibling));
	if(temp)
	ast->sibling->sibling 
	ast = createASTNode(reduceStmtAndFunctionDefs(ast->sibling->sibling->sibling->sibling));
	if(temp)
	ast->sibling->sibling->sibling 
	free(root->firstchild->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling);
	free(root->firstchild->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling);
	free(root->firstchild->sibling->sibling->sibling->sibling->sibling->sibling);
	free(root->firstchild->sibling->sibling->sibling->sibling);
	free(root->firstchild->sibling->sibling->sibling);
	free(root->firstchild->sibling);
	free(root->firstchild);
	free(root);
	return ast;
}	

ASTNode* reduceParameter_list(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceType(root->firstchild));
	if(temp)
	ast->node 
	ast->sibling = root->firstchild->sibling;
	ast = createASTNode(reduceRemainingList(root->firstchild->sibling->sibling));
	if(temp)
	ast->sibling 
	free(root);
	return ast;
}

ASTNode* reduceType(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	free(root);
	return ast;
}

ASTNode* reduceRemainingList(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceParameter_list(root->firstchild->sibling));
	if(temp)
	ast->node 
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceDeclarationStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceType(root->firstchild));
	if(temp)
	ast->node 
	ast = createASTNode(reduceVar_list(root->firstchild->sibling));
	if(temp)
	ast->sibling 
	free(root->firstchild->sibling->sibling);
	free(root);
	return ast;
}

ASTNode* reduceAssignmentStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild->sibling;
	ast = createASTNode(reduceLeftHandSide(root->firstchild));
	if(temp)
	ast->sibling 
	ast = createASTNode(reduceRightHandSide(root->firstchild->sibling->sibling));
	if(temp)
	ast->sibling->sibling 
	free(root->sibling->sibling->sibling);
	free(root);
	return ast;
}

ASTNode* reduceFunCallStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceFunctionCall(root->firstchild));
	if(temp)
	ast->node 
	free(root->firstchild->sibling);
	free(root);
	return ast;
}

ASTNode* reduceLeftHandSide(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node;
	switch(root->firstchild->token->symbol)
	{
		case TK_ID:  ast = root->firstchild; break;
		case TK_SQO:
		ast = createASTNode(reduceVar_list(root->firstchild->sibling));
		if(temp)
			 ast 
			 free(root->firstchild);
			 free(root->firstchild->sibling->sibling);
			 break;
		default: printf("Invalid Left Hand Side\n");
	}

	free(root);
	return ast;
}

ASTNode* reduceRightHandSide(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node;
	switch(root->firstchild->token->symbol)
	{
		ast = createASTNode(reduceFunctionCall(root->firstchild));
		if(temp)
		case 214: ast 
		ast = createASTNode(reduceSizeOf(root->firstchild));
		if(temp)
		case 215: ast 
		ast = createASTNode(reduceArithmeticExpression(root->firstchild));
		if(temp)
		case 220: ast 
		default: printf("Invalid Right Hand Side\n");
	}
	free(root);
	return ast;
}

ASTNode* reduceFunctionCall(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	ast = createASTNode(reduceInput_list(root->firstchild->sibling->sibling));
	if(temp)
	ast->sibling 
	free(root->firstchild->sibling);
	free(root->firstchild->sibling->sibling->sibling);
	free(root);
	return ast;
}

ASTNode* reduceSizeOf(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	ast->sibling = root->firstchild->sibling;
	free(root);
	return ast;
}

ASTNode* reduceConditionalStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceBooleanExpression(root->firstchild->sibling->sibling));
	if(temp)
	ast->node 
	ast = createASTNode(reduceOtherStmts(root->firstchild->sibling->sibling->sibling->sibling));
	if(temp)
	ast->sibling 
	ast = createASTNode(reduceElse_stmt(root->firstchild->sibling->sibling->sibling->sibling->sibling));
	if(temp)
	ast->sibling->sibling 
	free(root->firstchild->sibling->sibling->sibling->sibling->sibling->sibling);
	free(root->firstchild->sibling->sibling->sibling);
	free(root->firstchild->sibling);
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceElse_stmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceOtherStmts(root->firstchild->sibling));
	if(temp)
	ast->node 
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceOtherStmts(ParseTree root)
{
	return reduceStmtAndFunctionDefs(root);
}

ASTNode* reduceIoStmt(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	ast->sibling = root->firstchild->sibling->sibling;
	free(root->firstchild->sibling->sibling->sibling->sibling);
	free(root->firstchild->sibling->sibling->sibling);
	free(root->firstchild->sibling);
	free(root);
	return ast;
}

ASTNode* reduceArithmeticExpression(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node;
	switch(root->firstchild->token->symbol)
	{
		case 115:
		ast = createASTNode(reduceArith_operator(root->firstchild->sibling->sibling->sibling));
		if(temp)
			ast 
		 	ast = createASTNode(reduceArithmeticExpression(root->firstchild->sibling));
		 	if(temp)
		 	ast->sibling 
		 	ast = createASTNode(reduceArithmeticExpression(root->firstchild->sibling->sibling->sibling->sibling->sibling));
		 	if(temp)
		 	ast->sibling->sibling 
		 	free(root->firstchild->sibling->sibling->sibling->sibling->sibling->sibling);
		 	free(root->firstchild->sibling->sibling->sibling->sibling);
		 	free(root->firstchild->sibling->sibling);
		 	free(root->firstchild);
		 	break;
		case 222:
		ast = createASTNode(reduceSub_expression(root->firstchild);) 
		if(temp)
			ast 
			ast = createASTNode(reduceTerm(root->firstchild->sibling));
			if(temp)
			ast->sibling 
			break;
		default: printf("Invalid Arithmetic Expr\n");
	}
	free(root);
	return ast;
}

ASTNode* reduceTerm(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceOperator1(root->firstchild));
	if(temp)
	ast->node 
	ast = createASTNode(reduceSub_expression(root->firstchild->sibling));
	if(temp)
	ast->sibling 
	ast = createASTNode(reduceTerm(root->firstchild->sibling->sibling));
	if(temp)
	ast->sibling->sibling 
	free(root);
	return ast;
}

ASTNode* reduceSub_expression(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceVar(root->firstchild));
	if(temp)
	ast->node 
	ast = createASTNode(reduceFactor(root->firstchild->sibling));
	if(temp)
	ast->sibling 
	free(root);
	return ast;
}

ASTNode* reduceFactor(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceOperator2(root->firstchild));
	if(temp)
	ast->node 
	ast = createASTNode(reduceVar(root->firstchild->sibling));
	if(temp)
	ast->sibling 
	ast = createASTNode(reduceFactor(root->firstchild->sibling->sibling));
	if(temp)
	ast->sibling->sibling 
	free(root);
	return ast;
}

ASTNode* reduceOperator1(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	free(root);
	return ast;
}

ASTNode* reduceOperator2(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	free(root);
	return ast;
}

ASTNode* reduceArith_operator(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	free(root);
	return ast;
}

ASTNode* reduceBooleanExpression(ParseTree root)
{
		if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node;
	switch(root->firstchild->token->symbol)
	{
		case 115:
		ast = createASTNode(reduceLogicalOp(root->firstchild->sibling->sibling->sibling));
		if(temp)
			ast 
		 	ast = createASTNode(reduceBooleanExpression(root->firstchild->sibling));
		 	if(temp)
		 	ast->sibling 
		 	ast = createASTNode(reduceBooleanExpression(root->firstchild->sibling->sibling->sibling->sibling->sibling));
		 	if(temp)
		 	ast->sibling->sibling 
		 	free(root->firstchild->sibling->sibling->sibling->sibling->sibling->sibling);
		 	free(root->firstchild->sibling->sibling->sibling->sibling);
		 	free(root->firstchild->sibling->sibling);
		 	free(root->firstchild);
		 	break;
		case 228:
		ast = createASTNode(reduceRelationalOp(root->firstchild->sibling);) 
		if(temp)
			ast 
			ast = createASTNode(reduceVar(root->firstchild));
			if(temp)
			ast->sibling 
			ast = createASTNode(reduceVar(root->firstchild->sibling->sibling));
			if(temp)
			ast->sibling 
			break;
		case 132:
			ast = root->firstchild;
			ast = createASTNode(reduceBooleanExpression(root->firstchild->sibling->sibling));
			if(temp)
			ast->sibling 
		 	free(root->firstchild->sibling);
		 	free(root->firstchild->sibling->sibling->sibling);
		 	break;
		default: printf("Invalid Arithmetic Expr\n");
	}
	free(root);
	return ast;
}

ASTNode* reduceVar(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node;
	if(root->firstchild->token->symbol == 229)
		ast = createASTNode(reduceMatrixElement(root->firstchild));
	if(temp)
		ast 
	else ast = root->firstchild;
	free(root);
	return ast;
}

ASTNode* reduceMatrixElement(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceMatrixValues(root->firstchild->sibling));
	if(temp)
	ast->node 
	free(root->firstchild->sibling->sibling);
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceMatrixValues(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceRow_list(root->firstchild));
	if(temp)
	ast->node 
	ast = createASTNode(reduceRem_rowlist(root->firstchild->sibling));
	if(temp)
	ast->sibling 
	free(root);
	return ast;
}

ASTNode* reduceRow_list(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	ast = createASTNode(reduceRest_list(root->firstchild->sibling));
	if(temp)
	ast->sibling 
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceRest_list(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceRow_list(root->firstchild->sibling));
	if(temp)
	ast->node 
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceLogicalOp(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	free(root);
	return ast;
}

ASTNode* reduceRelationalOp(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	free(root);
	return ast;
}

ASTNode* reduceVar_list(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON) return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast->node = root->firstchild;
	ast = createASTNode(reduceMore_ids(root->firstchild->sibling));
	if(temp)
	ast->sibling 
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceMore_ids(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceVar_list(root->firstchild->sibling));
	if(temp)
	ast->node 
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceMoreStatements(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceStmtAndFunctionDefs(root->firstchild));
	if(temp)
	ast->node 
	free(root);
	return ast;
}

ASTNode* reduceRem_rowlist(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceMatrixValues(root->firstchild->sibling));
	if(temp)
	ast->node 
	free(root->firstchild);
	free(root);
	return ast;
}

ASTNode* reduceInput_list(ParseTree root)
{
	if(root == NULL || root->token->symbol == EPSILON)
		return NULL;
	ast = (ASTNode*)malloc(sizeof(ASTNode));
	ast->next = NULL; 
	ast = createASTNode(reduceVar_list(root->firstchild));
	if(temp)
	ast->node 
	free(root);
	return ast;
}