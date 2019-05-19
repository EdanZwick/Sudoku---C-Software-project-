/*
 * recStack.c
 *
 *	Auxiliary module that supplies an interface of a pseudo recursion stack that is used in solver module.
 *
 *
 *	Note: when working with sparse or large boards, the number of solutions tends to be very large.
 *	As the number of iterations is exponential to the number of solutions, this leads to *a lot* of stack actions.
 *	in this implementation the stack is a dynamically linked structure, thus each action causes a memory action (freeing or allocating).
 *	This is a large overhead in an already computational "heavy" algorithm.
 *
 *	Noticing that the stack depth is bonded by the total number of cells in the board, I tested an approach where the stack is implemented
 *	by a pre-allocated array (and a index indicating the top).
 *	This approach yielded considerably better performance even on relatively small boards.
 *	I consulted with T.A. Sulami which clarified that a linked implementation is required, so I left this original implementation.
 *	While probaly not as good, some improvement might be gained here by keeping a buffer containing unused nodes, instead of freeing and
 *	allocating on every action on the stack.
 *
 *  Created on: Apr 21, 2019
 *      Author: Edanz
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "recStack.h"

/*
 * Returns the index of previous modified cell, frees the topmost node, and changes the top pointer in stack to be new top.
 */
int pop (stack* s){
	int tmp;
	node* cur;
	if (s->cur == NULL){
		return -1;
	}
	cur = s->cur;
	s->cur = (cur->prev);
	tmp = (cur->index);
	free(cur);
	return tmp;
}

/*
 * Documents another move in the recursion stack: index is the last cell we tried to modify
 */
void push (stack* s, int index){
	node* new;
	assert((new = (node*) malloc(sizeof(node)))!=NULL && "warning");
	new->index = index;
	new->prev = s->cur;
	s->cur = new;
}
