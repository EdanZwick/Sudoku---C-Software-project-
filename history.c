/*
 * history.c
 *
 *	Auxiliary module for game.
 *	Manages a doubly linked list data structure recording move history for a board.
 *	The data structure itself is defined in the header file.
 *
 *  Created on: Feb 16, 2019
 *      Author: Edanz
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "history.h"


/*
 * initializes the history struct h.
 */
void createHistory(history* h){
	move* m;
	assert ((m=malloc(sizeof(move)))!=NULL  && "memory allocation error");
	m->old =0;
	m->new= 0;
	m->id = 0;
	m->index = -1;
	m->next = NULL;
	m->prev = NULL;
	h->cur = m;
	h->head = m;
	h->tail = m;
}

/*
 * Cleans all history.
 * next time just go incognito.
 */
void clearHistory(history* h){
	move *m;
	if (h->head == NULL){
		return;
	}
	m = (h->head);
	while (m->next != NULL){
		m = m->next;
		free(m->prev);
	}
	free(m);
}

/*
 * Clears all history after cur pointer.
 */
void clearTail(history* h){
	move *tmp = h->head;
	h->head = (h->cur)->next;
	clearHistory(h);
	h->head = tmp;
	h->tail = h->cur;
}

/*
 *	Records a move in board history.
 *	Receives a pointer to the boards history, metadata of a move that was made.
 *	newCmd - flag stating weather this change to the board is a start of new command or part of a bigger one (such as autofill)
 */
void record(history* h, int new, int old, int index, int newCmd){
	move* m;
	if (h->cur != h->tail){
		clearTail(h);
	}
	assert((m=malloc(sizeof(move)))!=NULL && "memory allocation error");
	m->index = index;
	m->new = new;
	m->old = old;
	m->next = NULL;
	(h->cur)->next = m;
	(m->prev) = (h->cur);
	(h->cur) = m;
	(h->tail) = m;
	m->id = ((m->prev)->id) + newCmd;
}

/*
 * Returns Id of last move that affected the board in it's current state.
 */
int curId(history* h){
	return ((h->cur)->id);
}

/*
 * Returns the ID of the last undone move
 */
int nextId(history* h){
	if (h->cur == h->tail){
		return -1;
	}
	return (((h->cur)->next)->id);
}

/*
 * Returns the ID of previous move
 */
int prevId(history* h){
	if (h->cur == h->head){
		return -1;
	}
	return (((h->cur)->prev)->id);
}

/*
 * Updates history pointer to undo a move and returns needed changes to boards.
 * arr[0] - value to set
 * arr[1] - index
 */
void past(history* h, int arr[]){
	if (h->cur == h->head){
		arr[0]=-1;
		return;
	}
	arr[0] = (h->cur)->old;
	arr[1] = (h->cur)->index;
	h->cur = (h->cur)->prev;
}

/*
 * Updates history pointer to undo a move and returns needed changes to boards.
 * arr[0] - value to set
 * arr[1] - index
 */
void future(history* h, int arr[]){
	if (h->cur == h->tail){
		arr[0]=-1;
		return;
	}
	h->cur = (h->cur)->next;
	arr[0] = (h->cur)->new;
	arr[1] = (h->cur)->index;
}

