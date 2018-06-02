// 2014B4A70530P
// GOVIND MITTAL

#include "lexerDef.h"
FILE* getStream(FILE *fp, char* buf);
void next(FileBuffer* filebuffer);
void removeComments(const char* testCaseFile, FileBuffer* filebuffer);
int matchKeyword(char* str);
struct lexeme* getNextToken(FileBuffer* filebuffer, struct lexeme* token);
void init_lexer(const char *testCaseFile, struct lexeme* token, FileBuffer* filebuffer);
void printTokenList(const char *testCaseFile, struct lexeme* token, FileBuffer* filebuffer);
char* printSymbolName(int symbol, char* name);