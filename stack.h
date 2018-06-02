struct casket
{
	struct elem* e;
	struct casket* next;
};

typedef struct 
{
	struct casket* head;
	int size;
}Stack;

void push(Stack* S, struct elem* e);
struct elem* pop(Stack* S);
void pushRule(Stack* S, struct elem* e);