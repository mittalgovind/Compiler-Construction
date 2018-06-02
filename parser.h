// 2014B4A70530P
// GOVIND MITTAL
#include "parserDef.h"

void calcPowers();
struct elem* createRhs(unsigned int symbol);
struct elem* populateRule(char* right);
Rule createRule(char* right);
void createLhs(int symbol, char* right, LHS* G[NON_TERMINALS]);
void addToLhs(int symbol, char* right, LHS* G[NON_TERMINALS]);
void populateGrammar(FILE* fp, LHS* G[NON_TERMINALS]);
void computeFirstandFollowSets(LHS* G[NON_TERMINALS]);
void createParseTable(LHS* G[NON_TERMINALS], Table T);
void printGrammar(LHS* G[NON_TERMINALS]);
ParseTree parseInputSourceCode(const char *testCaseFile, Table T, LHS* G[NON_TERMINALS]);
ParseTree parse(const char *testCaseFile);