// 2014B4A70530P
// GOVIND MITTAL
#define TRUE 				1
#define FALSE 				0
#define RULES 				80
#define TERMINALS 			39
#define TERMINALS_BASE 		100
#define NON_TERMINALS 		39
#define NON_TERMINALS_BASE 	201
#define RULE_LEN 			60

typedef unsigned long set;
// structures for Parse Table
struct elem
{
	unsigned int symbol;
	int isTerminal;
	struct elem* next;
};

struct rule
{
	set subfirstset;
	set subfollowset;
	set subterminals;
	struct elem* rhs;
	struct rule* next;
};
typedef struct rule* Rule;
typedef Rule** Table;

typedef struct 
{
	unsigned int symbol;
	set firstset;
	set followset;
	Rule info;
}LHS;
 
struct node
{
	int isLeaf;
	int visited;
	struct node* sibling;
	struct node* firstchild;
	struct node* parent;
	struct lexeme* token;
};
typedef struct node* ParseTree;
