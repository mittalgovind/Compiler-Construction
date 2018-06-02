#include "common.h"

FILE* getStream(FILE *fp, char* buf)
{
	// file-error checking
	memset(buf,0,BUF_SIZE+1);
	if(ferror(fp) != 0)
	{
		printf("Error while reading the file\n");
		exit(0);
	}

	int nread = fread(buf, sizeof(char), BUF_SIZE, fp);
	buf[nread] = '\0';
	return fp;
}

void next(FileBuffer* filebuffer)
{
	if(filebuffer->i == BUF_SIZE)
	{
		if(filebuffer->currBuf == 1)
		{
			filebuffer->fp = getStream(filebuffer->fp, filebuffer->buf1);
			filebuffer->currBuf = 2;
		}
		else 
		{
			filebuffer->fp = getStream(filebuffer->fp, filebuffer->buf2);
			filebuffer->currBuf = 1;
		}
		filebuffer->i = 0;
	}
	if(filebuffer->currBuf == 1)
		filebuffer->ch = filebuffer->buf1[filebuffer->i];
	else
		filebuffer->ch = filebuffer->buf2[filebuffer->i];
	filebuffer->i++;
}

void removeComments(const char* testCaseFile, FileBuffer* filebuffer)
{
	filebuffer->fp = fopen(testCaseFile, "r");
	filebuffer->i = 0;
	filebuffer->currBuf = 1;
	filebuffer->fp = getStream(filebuffer->fp, filebuffer->buf1);
	filebuffer->fp = getStream(filebuffer->fp, filebuffer->buf2);
	next(filebuffer);
	char clean[BUF_SIZE+1];
	int k = 0;
	do
	{
		if (filebuffer->ch == '#')
		{
			do
			{
				next(filebuffer);
			} while(filebuffer->ch != '\n');
			next(filebuffer);
		}
		else 
		{
			clean[k++] = filebuffer->ch;
			if(k == BUF_SIZE)
			{
				clean[k] = '\0';
				printf("%s", clean);
				k = 0;
			}
			next(filebuffer);
		}
	} while(filebuffer->ch);

	clean[k] = '\0';
	if(k != 0)
	{
		printf("%s", clean);
	}
	fclose(filebuffer->fp);
}

int matchKeyword(char* str)
{
	int j;
	char keywords[11][10] = {
							 "int", "real", "string", "matrix",
							 "if", "else", "endif", "read", 
							 "print", "function", "end"
							};

	for(j = 0; j < 11; j++)
		if (strcmp(keywords[j], str) == 0)
			break;

	if (j < 11)
	{
		switch(j)
		{
			case 0 : j = TK_INT; break;
			case 1 : j = TK_REAL; break;
			case 2 : j = TK_STRING; break;
			case 3 : j = TK_MATRIX; break;
			case 4 : j = TK_IF; break;
			case 5 : j = TK_ELSE; break;
			case 6 : j = TK_ENDIF; break;
			case 7 : j = TK_READ; break;
			case 8 : j = TK_PRINT; break;
			case  9 : j = TK_FUNCTION; break;
			case  10 : j = TK_END; break;
		}
	}
	else 
		j = -1;
	return j;
}


void report_error(int errno, int line_no, char* bad_token)
{
	switch(errno)
	{
		case SYMBOLMISMATCH: printf("%d. Lexical Error: Symbol Not Matched: %s\n", line_no, bad_token);
							break;
		case PATTERNMISMATCH: printf("%d. Lexical Error: Pattern Does not match %s\n", line_no, bad_token);
							break;
		case IDTOOLONG: printf("%d. Lexical Error: ID too long %s\n", line_no, bad_token);
							break;
		case BRACKETMISSING: printf("%d. Lexical Error: Bracket missing after function definition/call%s\n", line_no, bad_token);
							break;
		case QUOTEMISSING: printf("%d. Lexical Error: Closing Quote Missing%s\n", line_no, bad_token);
							break;
		case ERROR: printf("%d. Lexical Error: Bracket missing after function definition/call%s\n", line_no, bad_token);
	}
}
void assignToken(struct lexeme* token, int symbol, char* info, int line_no)
{
	token->symbol = symbol;
	if(info == NULL)
		memset(token->info, 0, ID_LEN);
	else strcpy(token->info, info);
	token->line_no = line_no;
}

struct lexeme* getNextToken(FileBuffer* filebuffer, struct lexeme* token)
{
	int k = 0;
	char temp[ID_LEN];
	
	// DFA implementation
	switch(filebuffer->ch)
	{
		// comment skipping
		case '#'	:
		next(filebuffer);
		while(filebuffer->ch != '\n')
			next(filebuffer);

		// newline
		case '\n'	: 
		filebuffer->line_count++;
		next(filebuffer);
		return  getNextToken(filebuffer, token);
		break; 
		
		// blank space
		case ' '	: 
		case '\r'	: 
		case '\t'	: 
		next(filebuffer);
		return  getNextToken(filebuffer, token);
		break;
		
		// plus 
		case '+'	:
		assignToken(token, TK_PLUS, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// multiply
		case '*'	: 
		assignToken(token, TK_MUL, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// subtract
		case '-'	:
		assignToken(token, TK_MINUS, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// divide
		case '/'	:
		assignToken(token, TK_DIV, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// greater
		case '>'	:
		next(filebuffer);
		if(filebuffer->ch == '=')
		{
			assignToken(token, TK_GE, NULL, filebuffer->line_count);
			next(filebuffer);
		}
		else 
		{
			assignToken(token, TK_GT, NULL, filebuffer->line_count);
		}
		break;
		
		// less
		case '<'	:
		next(filebuffer);
		if(filebuffer->ch == '=')
		{
			assignToken(token, TK_LE, NULL, filebuffer->line_count);
			next(filebuffer);
		}
		else 
			assignToken(token, TK_LT, NULL, filebuffer->line_count);
		break;

		// assignment, equality and non-equality 
		case '='	:
		next(filebuffer);
		if (filebuffer->ch == '=')
		{
			assignToken(token, TK_EQ, NULL, filebuffer->line_count);
			next(filebuffer);
		}
		else
		{  
			if(filebuffer->ch == '/' )
			{
				next(filebuffer);
				if (filebuffer->ch == '=')
				{
					assignToken(token, TK_NE, NULL, filebuffer->line_count);
					next(filebuffer);
				}
				else report_error( SYMBOLMISMATCH, filebuffer->line_count, &(filebuffer->ch));
			}
			else 
				assignToken(token, TK_ASSIGNOP, NULL, filebuffer->line_count);
		}
		break;

		// left paranthesis
		case '('	:
		assignToken(token, TK_OP, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// right paranthesis
		case ')'	:
		assignToken(token, TK_CL, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// size operator
		case '@'	:
		assignToken(token, TK_SIZE, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// left bracket
		case '['	:
		assignToken(token, TK_SQO, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// right bracket
		case ']'	:
		assignToken(token, TK_SQC, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// comma
		case ','	:
		assignToken(token, TK_COMMA, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// semi-colon
		case ';'	:
		assignToken(token, TK_SEMICOLON, NULL, filebuffer->line_count);
		next(filebuffer);
		break;
		
		// logical operators
		case '.'	:
		next(filebuffer);
		k = 0;
		label:
		while(filebuffer->ch != '.' && k < 4)
		{
			temp[k++] = filebuffer->ch;
			next(filebuffer);
		}
		temp[k] = '\0';
		next(filebuffer);
		if (strcmp(temp, "and"))
			assignToken(token, TK_AND, NULL, filebuffer->line_count);
		else
		{
			if (strcmp(temp, "or"))
				assignToken(token, TK_OR, NULL, filebuffer->line_count);
			else
			{
				 if (strcmp(temp, "not"))
					assignToken(token, TK_NOT, NULL, filebuffer->line_count);
				else report_error(PATTERNMISMATCH, filebuffer->line_count, temp);
			}
		}
		break;
		

		// functions
		case '_' :
		k = 0;
		next(filebuffer);
		if (isalpha(filebuffer->ch))
		{
			temp[k++] = filebuffer->ch;
			next(filebuffer);
			while(isalpha(filebuffer->ch) || isdigit(filebuffer->ch))
			{
				temp[k++] = filebuffer->ch;
				next(filebuffer);
			}
			if(filebuffer->ch == '[' ||  filebuffer->ch == '(')
			{
				temp[k] = '\0';
				if (strcmp(temp, "main") == 0)
					assignToken(token, TK_MAIN, NULL, filebuffer->line_count);
				else 
					assignToken(token, TK_FUNID, temp, filebuffer->line_count);
			}
			else report_error(BRACKETMISSING, filebuffer->line_count, temp);
		}
		else report_error(PATTERNMISMATCH, filebuffer->line_count, temp);
		break;
		
		// identifier
		case 'a'	:
		case 'b'	:
		case 'c'	:
		case 'd'	:
		case 'e'	:
		case 'f'	:
		case 'g'	:
		case 'h'	:
		case 'i'	:
		case 'j'	:
		case 'k'	:
		case 'l'	:
		case 'm'	:
		case 'n'	:
		case 'o'	:
		case 'p'	:
		case 'q'	:
		case 'r'	:
		case 's'	:
		case 't'	:
		case 'u'	:
		case 'v'	:
		case 'w'	:
		case 'x'	:
		case 'y'	:
		case 'z'	:
		case 'A'	:
		case 'B'	:
		case 'C'	:
		case 'D'	:
		case 'E'	:
		case 'F'	:
		case 'G'	:
		case 'H'	:
		case 'I'	:
		case 'J'	:
		case 'K'	:
		case 'L'	:
		case 'M'	:
		case 'N'	:
		case 'O'	:
		case 'P'	:
		case 'Q'	:
		case 'R'	:
		case 'S'	:
		case 'T'	:
		case 'U'	:
		case 'V'	:
		case 'W'	:
		case 'X'	:
		case 'Y'	:
		case 'Z'	:
		k = 0;
		while(isalpha(filebuffer->ch) && k < ID_LEN)
		{
			temp[k++] = filebuffer->ch;
			next(filebuffer);
		}
		if (k == ID_LEN)
		{
			report_error(IDTOOLONG, filebuffer->line_count, temp);
			break;
		}
		if(!isdigit(filebuffer->ch))
		{
			temp[k] = '\0';
			token->symbol = -1;

			// matfilebuffer->ching the ID with the list of keywords
			assignToken(token, matchKeyword(temp), NULL, filebuffer->line_count);
			if(token->symbol == -1)
				assignToken(token, TK_ID, temp, filebuffer->line_count);
			else 
				break;
		}
		else 
		{
			temp[k++] = filebuffer->ch;
			next(filebuffer);
			temp[k] = '\0';
			assignToken(token, TK_ID, temp, filebuffer->line_count);
		}
		break;

		// integers and reals
		case '0'	:
		case '1'	:
		case '2'	:
		case '3'	:
		case '4'	:
		case '5'	:
		case '6'	:
		case '7'	:
		case '8'	:
		case '9'	:
		k = 0;
		while(isdigit(filebuffer->ch))
		{
			temp[k++] = filebuffer->ch;
			next(filebuffer);
		}

		if (filebuffer->ch == '.')
		{
			next(filebuffer);
			if(isdigit(filebuffer->ch))
			{
				temp[k++] = filebuffer->ch;
				next(filebuffer);
				if(isdigit(filebuffer->ch))
				{
					temp[k++] = filebuffer->ch;
					next(filebuffer);
					temp[k] = '\0';
				}
				assignToken(token, TK_RNUM, temp, filebuffer->line_count);
			}
			else goto label; 
		}
		else
		{
			temp[k] = '\0';
			assignToken(token, TK_NUM, temp, filebuffer->line_count);
		}
		break;

		//strings
		case '"'	:
		k = 0;
		next(filebuffer);
		while((filebuffer->ch >= 'a' && filebuffer->ch <= 'z') || isspace(filebuffer->ch))
		{
			temp[k++] = filebuffer->ch;
			next(filebuffer);
		}
		if(filebuffer->ch == '"')
		{
			temp[k] = '\0';
			assignToken(token, TK_STR, temp, filebuffer->line_count);
			next(filebuffer);
		}
		else report_error(QUOTEMISSING, filebuffer->line_count, temp);
		break;
			
		case '\0':
		 	token->symbol = EOF;
		 	break;

		default: report_error(ERROR, filebuffer->line_count, temp);
	}
	return token;
}

char* printSymbolName(int symbol, char* name)
{
	switch(symbol)
	{
		case 101 : strcpy(name, "ASSIGNOP"); break;
		case 102 : strcpy(name, "FUNID"); break;
		case 103 : strcpy(name, "ID"); break;
		case 104 : strcpy(name, "NUM"); break;
		case 105 : strcpy(name, "RNUM"); break;
		case 106 : strcpy(name, "STR"); break;
		case 107 : strcpy(name, "END"); break;
		case 108 : strcpy(name, "INT"); break;
		case 109 : strcpy(name, "REAL"); break;
		case 110 : strcpy(name, "STRING"); break;
		case 111 : strcpy(name, "MATRIX"); break;
		case 112 : strcpy(name, "MAIN"); break;
		case 113 : strcpy(name, "SQO"); break;
		case 114 : strcpy(name, "SQC"); break;
		case 115 : strcpy(name, "OP"); break;
		case 116 : strcpy(name, "CL"); break;
		case 117 : strcpy(name, "SEMICOLON"); break;
		case 118 : strcpy(name, "COMMA"); break;
		case 119 : strcpy(name, "IF"); break;
		case 120 : strcpy(name, "ELSE"); break;
		case 121 : strcpy(name, "ENDIF"); break;
		case 122 : strcpy(name, "READ"); break;
		case 123 : strcpy(name, "PRINT"); break;
		case 124 : strcpy(name, "FUNCTION"); break;
		case 125 : strcpy(name, "PLUS"); break;
		case 126 : strcpy(name, "MUL"); break;
		case 127 : strcpy(name, "MINUS"); break;
		case 128 : strcpy(name, "DIV"); break;
		case 129 : strcpy(name, "SIZE"); break;
		case 130 : strcpy(name, "AND"); break;
		case 131 : strcpy(name, "OR"); break;
		case 132 : strcpy(name, "NOT"); break;
		case 133 : strcpy(name, "LT"); break;
		case 134 : strcpy(name, "LE"); break;
		case 135 : strcpy(name, "EQ"); break;
		case 136 : strcpy(name, "GT"); break;
		case 137 : strcpy(name, "GE"); break;
		case 138 : strcpy(name, "NE"); break;

		case 201 : strcpy(name, "mainFunction"); break;
		case 202 : strcpy(name, "stmtAndFunctionDefs"); break;
		case 203 : strcpy(name, "stmtOrFunctionDef"); break;
		case 204 : strcpy(name, "stmt"); break;
		case 205 : strcpy(name, "functionDef"); break;
		case 206 : strcpy(name, "parameter_list"); break;
		case 207 : strcpy(name, "type"); break;
		case 208 : strcpy(name, "remainingList"); break;
		case 209 : strcpy(name, "declarationStmt"); break;
		case 210 : strcpy(name, "assignmentStmt"); break;
		case 211 : strcpy(name, "funCallStmt"); break;
		case 212 : strcpy(name, "leftHandSide"); break;
		case 213 : strcpy(name, "rightHandSide"); break;
		case 214 : strcpy(name, "functionCall"); break;
		case 215 : strcpy(name, "sizeOf"); break;
		case 216 : strcpy(name, "conditionalStmt"); break;
		case 217 : strcpy(name, "else_stmt"); break;
		case 218 : strcpy(name, "otherStmts"); break;
		case 219 : strcpy(name, "ioStmt"); break;
		case 220 : strcpy(name, "arithmeticExpression"); break;
		case 221 : strcpy(name, "term"); break;
		case 222 : strcpy(name, "sub_expression"); break;
		case 223 : strcpy(name, "factor"); break;
		case 224 : strcpy(name, "operator1"); break;
		case 225 : strcpy(name, "operator2"); break;
		case 226 : strcpy(name, "arith_operator"); break;
		case 227 : strcpy(name, "booleanExpression"); break;
		case 228 : strcpy(name, "var"); break;
		case 229 : strcpy(name, "matrixElement"); break;
		case 230 : strcpy(name, "matrixValues"); break;
		case 231 : strcpy(name, "row_list"); break;
		case 232 : strcpy(name, "rest_list"); break;
		case 233 : strcpy(name, "logicalOp"); break;
		case 234 : strcpy(name, "relationalOp"); break;
		case 235 : strcpy(name, "var_list"); break;
		case 236 : strcpy(name, "more_ids"); break;
		case 237 : strcpy(name, "moreStatements"); break;
		case 238 : strcpy(name, "rem_rowlist"); break;
		case 239 : strcpy(name, "input_list"); break;
		
		case -1  : strcpy(name, "-Finish-"); break;
		default : printf("incorrect symbol given for printing : %d\n", symbol);
	}
	return name;
}

void init_lexer(const char *testCaseFile, struct lexeme* token, FileBuffer* filebuffer)
{
	filebuffer->fp = fopen(testCaseFile, "r");
	filebuffer->i = 0;
	filebuffer->line_count = 1;
	filebuffer->currBuf = 1;
	filebuffer->fp = getStream(filebuffer->fp, filebuffer->buf1);
	filebuffer->fp = getStream(filebuffer->fp, filebuffer->buf2);
	next(filebuffer);
}

void printTokenList(const char *testCaseFile, struct lexeme* token, FileBuffer* filebuffer)
{
	init_lexer(testCaseFile, token, filebuffer);
	char temp[ID_LEN];
	int i = 0;
	printf("LINE\t\tTOKEN\t\tLEXEME\n");
	do
	{
		token = getNextToken(filebuffer, token);
		printSymbolName(token->symbol, temp);
		printf("%d\t\t%s\t",token->line_no, temp);
		if(strlen(temp) < 8)printf("\t");
		if(token->symbol == TK_NUM)
			printf("%d\n", atoi(token->info));
		else if(token->symbol == TK_RNUM)
			printf("%.2f\n", atoi(token->info)/100.0);
		else if(token->symbol == TK_STR)
			printf("%s\n", token->info);
		else printf("----\n");
		i++;
	}while(token->symbol != EOF);

	fclose(filebuffer->fp);
	
}