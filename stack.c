// 2014B4A70530P
// GOVIND MITTAL

#include "common.h"

void push(Stack* S, struct elem* e)
{
	struct casket* newnode = (struct casket*)malloc(sizeof(struct casket));
	newnode->next = S->head;
	newnode->e = (struct elem*)malloc(sizeof(struct elem));
	newnode->e->symbol = e->symbol;
	newnode->e->isTerminal = e->isTerminal;
	newnode->e->next = e->next;
	S->head = newnode;
	S->size++;
}

struct elem* pop(Stack* S)
{
	struct casket* temp = S->head;
	S->head = S->head->next;
	S->size--;
	temp->next = NULL;
	return temp->e;
}

void pushRule(Stack* S, struct elem* e)
{
	if(e == NULL || e->symbol == EPSILON) return;
	pushRule(S, e->next);
	push(S, e);
}
