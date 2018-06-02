// 2014B4A70530P
// GOVIND MITTAL
#include "common.h"

int main(int argc, char const *argv[])
{
	int choice, parsetreesize, astsize;
	struct lexeme* token = (struct lexeme*)malloc(sizeof(struct lexeme));
    FileBuffer* filebuffer = (FileBuffer*)malloc(sizeof(FileBuffer));
	ParseTree pt = NULL;
	Node* ast = NULL;

	if(argc != 3)
	{
		printf("Insufficient number of arguments\n");
		printf("Usage: ./stage1exe testcase.txt code.asm\n");
		exit(0);
	}

	printf("\nFIRST and FOLLOW sets have been automated\n");
	printf("Lexer, Parser, AST, Symbol Table and Semantic Analysis modules implemented\n");
	do
	{
		printf("\n");
		printf("Choose an option\n");
		printf("1. Print Token List\n");
		printf("2. For performing Syntax Analysis and produce Parse Tree\n");
		printf("3. Print AST to the Output File \n");
		printf("4. Print compression ratio\n");
		printf("5. Print Symbol Table\n");
		printf("6. Perform Semantic Analysis (includes syntax analysis)\n");
		printf("7. Do code generation\n");
		printf("0. EXIT\n");
		printf("Please enter your choice :");
		scanf("%d", &choice);
		printf("\n\n");
		switch(choice)
		{
			case 1 : 
				printTokenList(argv[1], token, filebuffer);
				break;
			case 2 :
				pt = parse(argv[1]);
				printf("lexemeCurrentNode \t\tlineno \t\ttoken \t\tvalueIfNumber \t\tparentNodeSymbol \t\tisLeafNode(yes/no) \t\tNodeSymbol\n");
				inorder_traversal(pt);
				break;

			case 3 : 
				if(pt == NULL)
				{
					pt = parse(argv[1]);
					printf("lexemeCurrentNode \t\tlineno \t\ttoken \t\tvalueIfNumber \t\tparentNodeSymbol \t\tisLeafNode(yes/no) \t\tNodeSymbol\n");
					inorder_traversal(pt);
				}
				ast = reduceMainFunction(pt);
				printf("PRINTING AST INORDER\n");
				printAST(ast);
				break;

			case 4 : 
				if(!ast)
				{
				 	printf("Please run option 3, first\n"); 
				 	break;
				}
				parsetreesize = getTreeSize() * (int)sizeof(struct node);
				printf("Parse tree: Number of nodes = %d \t Allocated Memory = %d bytes\n", getTreeSize(), parsetreesize);
				astsize = getAstSize() * (int)sizeof(struct astnode);
				printf("Abstract Syntax Tree: nodes = %d \t Allocated Memory = %d bytes\n", getAstSize(), astsize );
				printf("Compression Percentage = %.2f percent \n", ((parsetreesize-astsize)*100.0)/parsetreesize);
				break;
			case 5 : break;
			case 6 : break;
			case 7 : break;
			case 8 : break;
			case 9 : break;
			case 0 : 
				printf("Terminating now! Thankyou.\n" ); 	
				free(filebuffer);
				free(token);
				exit(0);
			default : printf("Invalid option, please try again\n");
		}
	}while(1);
	return EXIT_SUCCESS;
}