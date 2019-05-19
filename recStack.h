/*
 * recStack.h
 *
 *	Auxilary model for solver. supplies an interface of a pseudo recursion stack that is used in solver module.
 *
 *
 *  Created on: Apr 21, 2019
 *      Author: Edanz
 */

#ifndef RECSTACK_H_
#define RECSTACK_H_

/*
 * Used as "recursion stack" for exhaustive backtracking.
 * index- index of last modified cell
 * prev- pointer to the previous node in stack
 */
typedef struct bullet{
	int index;
	struct bullet *prev;
} node;

typedef struct magazine{
	node* cur;
} stack;

/*
 * Documents another move in the recursion stack: index is the last cell we tried to modify
 */
void push (stack* s, int index);

/*
 * Returns the index of previous modified cell, frees the topmost node, and changes the top pointer in stack to be new top.
 */
int pop (stack* s);

#endif /* RECSTACK_H_ */
