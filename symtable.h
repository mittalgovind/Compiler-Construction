#define P 5381
#define MAX_CNT 10
#define FUN_ID_LEN 20
#define REAL_WIDTH 8
#define INT_WIDTH 4
#define CHAR_WIDTH 1

typedef struct symboltableelement
{
	int type;
	int offset;
	int width;
	char info[ID_LEN];
	int used;
	int matrows;
	int matcols;
}SymbolTableElement;

typedef struct symboltable
{
	struct symboltable *parent;
	SymbolTableElement **elements;
	int nesting;
	char scope[FUN_ID_LEN];
	int t;
	int n;
}SymbolTable;

typedef struct functiontableelement
{
	char info[FUN_ID_LEN];
	int no_of_inputs;
	int no_of_outputs;
	int *input_types;
	int *output_types;
	char** input_ids;
	char** output_ids;
	SymbolTable* ST;
	struct functiontable* parent;
	struct functiontable* FT;
}FunctionTableElement;

typedef struct functiontable
{
	FunctionTableElement** elements;
	struct functiontableelement* parent;
	int t;
	int n;
}FunctionTable;

int hash(char* str, int t);
SymbolTable* createSymbolTable(SymbolTable* parent, char* scope);
int recordCreate(SymbolTableElement** record, int, int type, int offset, char* info, int);
void recordCopy(SymbolTableElement** dst, int, SymbolTableElement* src);
void reHash(SymbolTable* ST);
SymbolTableElement* findSymbol(SymbolTable* ST, char* str);
int insertSymbol(SymbolTable* ST, int type, int offset, char* str, int used);
void printSymbolTable(SymbolTable* ST);
FunctionTable* createFunctionTable(FunctionTableElement* );
FunctionTableElement* funRecordCreate(FunctionTableElement*, FunctionTable*, FunctionTableElement** record, int hashCode, char* info, 
	int no_of_inputs, int no_of_outputs, int* input_types, int* output_types, char** input_ids, char** output_ids, SymbolTable* ST);
void funRecordCopy(FunctionTableElement** dst, int hashCode, FunctionTableElement* src);
void reHashFunctionTable(FunctionTable* FT);
FunctionTableElement* findFunction(FunctionTable* FT, char* str);
FunctionTableElement* insertFunction(FunctionTableElement* ,FunctionTable* FT, char* info, 
	int no_of_inputs, int no_of_outputs, int* input_types,
	int* output_types, char** input_ids, char** output_ids, SymbolTable* ST);