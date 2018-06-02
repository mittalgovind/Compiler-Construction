#include "common.h"

int hash(char* str, int t)
{
	int i, sum = 0;
	for(i = 0; str[i]; i++)
		sum += P * (int)str[i] + 19;
	return sum%((int)pow(2,t));
}

SymbolTable* createSymbolTable(SymbolTable* parent, char* scope)
{
	SymbolTable* ST = (SymbolTable*)malloc(sizeof(SymbolTable));
	ST->t = 4;
	ST->elements = (SymbolTableElement**)malloc(((int)pow(2, ST->t))*sizeof(SymbolTableElement*));
	ST->n = 0;
	for(int i = 0; i < (int)pow(2, ST->t); i++)
		ST->elements[i] = NULL;
	ST->parent = parent;
	if(parent)
		ST->nesting = parent->nesting + 1;
	else ST->nesting = 1;
	strcpy(ST->scope, scope);
	return ST;
}

int recordCreate(SymbolTableElement** record, int hashCode, 
			int type, int offset, char* info, int used)
{
	record[hashCode] = (SymbolTableElement*)malloc(sizeof(SymbolTableElement));
	record[hashCode]->type = type;
	switch(type)
	{
		case TK_INT: 
			record[hashCode]->width = INT_WIDTH;
			break;
		case TK_STRING: 
			record[hashCode]->width = CHAR_WIDTH * strlen(info);
			break;
		case TK_REAL: 
			record[hashCode]->width = REAL_WIDTH;
			break;
		case TK_MATRIX: 
			record[hashCode]->width = 8; // matrix is assumed to be a pointer 
			break;
	}
	record[hashCode]->offset = offset;
	memset(record[hashCode]->info, '\0', ID_LEN);
	strcpy(record[hashCode]->info, info);
	record[hashCode]->used = used;
	return record[hashCode]->width;
}

void recordCopy(SymbolTableElement** dst, int hashCode, SymbolTableElement* src)
{
	dst[hashCode] = (SymbolTableElement*)malloc(sizeof(SymbolTableElement));
	dst[hashCode]->type = src->type;
	dst[hashCode]->offset = src->offset;
	dst[hashCode]->width = src->width;
	dst[hashCode]->used = src->used;
	strcpy(dst[hashCode]->info, src->info);
}

void reHash(SymbolTable* ST)
{
	int i, hashCode;
	SymbolTableElement** newElements = (SymbolTableElement**)
				malloc(((int)pow(2, ST->t+1))*sizeof(SymbolTableElement*));

	for (i = 0; i < (int)pow(2, ST->t+1); ++i)
		newElements[i] = NULL;

	for (i = 0; i < (int)pow(2, ST->t); ++i)
		if(ST->elements[i] != NULL)
		{
			hashCode = hash(ST->elements[i]->info, ST->t+1);
			recordCopy(newElements, hashCode, ST->elements[i]);
		}

	for (i = 0; i < (int)pow(2, ST->t); ++i)
		free(ST->elements[i]);
	free(ST->elements);
	ST->elements = newElements;
	ST->t++;
}

SymbolTableElement* findCurrScope(SymbolTable* ST, char* str)
{
	if(ST && ST->elements[hash(str, ST->t)] &&
		strcmp(ST->elements[hash(str, ST->t)]->info,str) == 0)
		return ST->elements[hash(str, ST->t)];
	else return NULL;
}

// tries to find the symbol in its complete scope
SymbolTableElement* findSymbol(SymbolTable* ST, char* str)
{
	SymbolTable* temp = ST;
	SymbolTableElement* t;
	while(temp)
	{
		t = findCurrScope(temp, str);
		if(t) 
			return t;
		temp = temp->parent;
	}
	return NULL;
}

int insertSymbol(SymbolTable* ST, int type, int offset, char* str, int used)
{
	int hashCode;
	if (findCurrScope(ST, str))
	{
		printf("The variable %s is already present in current scope =%s \n", str, ST->scope);
		return 0;
	}
	else
	{
		hashCode = hash(str, ST->t);
		while(ST->elements[hashCode] != NULL)
			reHash(ST);
		ST->n++;
		return recordCreate(ST->elements, hashCode, type, offset, str, used);
	}
	return 0;
}

void printSymbolTable(SymbolTable* ST){
	SymbolTableElement* element;
	for (int i = 0; i < (int)pow(2, ST->t); ++i)
	{
		if(ST->elements[i] == NULL)
			continue;
		element = ST->elements[i];
		if(ST->parent == NULL)
		printf("%s \t %s \t %d \t NULL %s %d %d\n",
			element->info, ST->scope, ST->nesting, printSymbolName(element->type , " "), element->width, element->offset);
		else
		printf("%s \t %s \t %d \t %s %s %d %d\n",
			element->info, ST->scope, ST->nesting, ST->parent->scope, printSymbolName(element->type , " "), element->width, element->offset);
	}
}
// FunctionTable* parent
FunctionTable* createFunctionTable(FunctionTableElement* parent)
{
	FunctionTable* FT = (FunctionTable*)malloc(sizeof(FunctionTable));
	FT->t = 2;
	FT->elements = (FunctionTableElement**)malloc(((int)pow(2, FT->t))*sizeof(FunctionTableElement*));
	FT->n = 0;
	for(int i = 0; i < (int)pow(2, FT->t); i++)
		FT->elements[i] = NULL;
	FT->parent = parent;
	return FT;
}

FunctionTableElement* funRecordCreate(FunctionTableElement* parentelement, FunctionTable* parent, FunctionTableElement** record, int hashCode, char* info, 
					int no_of_inputs, int no_of_outputs, int* input_types, int* output_types, char** input_ids,
						 char** output_ids, SymbolTable* ST)
{
	record[hashCode] = (FunctionTableElement*)malloc(sizeof(FunctionTableElement));
	strcpy(record[hashCode]->info, info);
	record[hashCode]->no_of_inputs = no_of_inputs;
	record[hashCode]->no_of_outputs = no_of_outputs;
	record[hashCode]->input_types = (int*)malloc(sizeof(int)*no_of_inputs);
	record[hashCode]->output_types = (int*)malloc(sizeof(int)*no_of_outputs);
	record[hashCode]->input_types = input_types;
	record[hashCode]->output_types = output_types;
	record[hashCode]->input_ids = input_ids;
	record[hashCode]->output_ids = output_ids;
	record[hashCode]->FT = createFunctionTable(parentelement);
	record[hashCode]->parent = parent;
	record[hashCode]->ST = ST;
	return record[hashCode];
}

void funRecordCopy(FunctionTableElement** dst, int hashCode, FunctionTableElement* src)
{
	dst[hashCode] = (FunctionTableElement*)malloc(sizeof(FunctionTableElement));
	dst[hashCode]->no_of_outputs = src->no_of_outputs;
	dst[hashCode]->no_of_inputs = src->no_of_inputs;
	dst[hashCode]->input_types = src->input_types;
	dst[hashCode]->output_types = src->output_types;
	dst[hashCode]->FT = src->FT;
	dst[hashCode]->ST = src->ST;
	dst[hashCode]->parent = src->parent;
	strcpy(dst[hashCode]->info, src->info);
}

void reHashFunctionTable(FunctionTable* FT)
{
	int i, hashCode;
	FunctionTableElement** newElements = (FunctionTableElement**)
				malloc(((int)pow(2, FT->t+1))*sizeof(FunctionTableElement*));

	for (i = 0; i < (int)pow(2, FT->t+1); ++i)
		newElements[i] = NULL;

	for (i = 0; i < (int)pow(2, FT->t); ++i)
		if(FT->elements[i] != NULL)
		{
			hashCode = hash(FT->elements[i]->info, FT->t+1);
			funRecordCopy(newElements, hashCode, FT->elements[i]);
		}

	for (i = 0; i < (int)pow(2, FT->t); ++i)
		free(FT->elements[i]);
	free(FT->elements);
	FT->elements = newElements;
	FT->t++;
}

FunctionTableElement* findFunction(FunctionTable* FT, char* str)
{
	if(FT && FT->elements[hash(str, FT->t)] &&
		strcmp(FT->elements[hash(str, FT->t)]->info,str) == 0)
		return FT->elements[hash(str, FT->t)];
	return 0;
}

FunctionTableElement* insertFunction(FunctionTableElement* parentelement, FunctionTable* FT, char* info, 
	int no_of_inputs, int no_of_outputs, int* input_types,
	int* output_types, char** input_ids, char** output_ids, SymbolTable* ST)
{
	int hashCode;
	if (findFunction(FT, info))
	{
		printf("The function %s is already present in current scope. Function overloading is not allowed.\n",
		 info);
		return NULL;
	}
	else
	{
		hashCode = hash(info, FT->t);
		while(FT->elements[hashCode] != NULL)
			reHashFunctionTable(FT);
		FT->n++;
		return funRecordCreate(parentelement, FT, FT->elements, hashCode, info, 
			no_of_inputs, no_of_outputs, input_types, output_types,
			input_ids, output_ids, ST);
	}
	return 0;
}