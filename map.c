/*
 * map.c
 *
 *  Created on: Apr 18, 2019
 *      Author: Edanz
 *
 *	Supplies a data structure that helps create efficient linear programs. the data structure maps between a cell
 *	and the variables that represent possible assignments to it, and are sent to the optimizer.
 *
 *	Feeding the optimizer with a n^3 variables is impractical, so we will only feed non-trivial options to it.
 *	i.e. variables representing possible placement for cell
 * (As opposed to variables for cells that already contain a value or variables representing obvious invalid placements)
 *
 *	This creates a problem of keeping track on what variable in the model represents what cell and which specific value.
 *	My solution to this problem is to keep a custom data structure mapping between the cells and the variables that represent them.
 *	The data structure itself is implemented in the header file.
 *	This module will hold implementation of functions that operate on this structure and keep it abstract from the rest of the project.
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "solver.h"
#include "map.h"

void pack(cell* c, mnode** cur,int j);
void destroyCell(mnode* n);

/*
 * Initializes a mapping for board b, and Returns a pointer to the new data structure created.
 *
 * Receives a matrix representation of the board and dimensions.
 * Creates the data structure containing the data needed for each cell- what numbers are possible assignments
 * and index of first variable belonging each cell.
 *
 * If it's obvious during the mapping that no solution is possible, we will return NULL
 *
 * Note that it wouldv'e saved us some CPU time if we built our constraints to the optimizer together with the map
 * (each time we create a variable we already now exactly what constraints it will appear in).
 * This though will cause significant memory usage (we can't despatch the constraints until we went over the entire board-
 * so we will have to keep O(3 * n^2) memory just for the constraints.
 * Current approach will add O(3*n) to the running time, but will save the O(n^2) memory.
 *
 */
map* createMap(int* b, int blockw, int blockh){
	int i, j, maxVal = (blockw*blockh), size = maxVal * maxVal;
	map* m;
	mnode* cur;
	cell* c;
	/*allocate space for the map, and for the cells themselves*/
	assert((m = (map*) (calloc(1,sizeof(map))))!=NULL && "Memory allocation error");
	m->size = size;
	m->total = 0;
	assert(((m->cells) =  calloc(size,sizeof(mnode)))!=NULL && "Memory allocation error");
	/*start filling map*/
	for (i = 0 ; i < size ; i++){
		((m->cells)[i]).index = m->total;
		((m->cells)[i]).num = 0;
		c = &(m->cells)[i];
		if (b[i]!=0){
			continue; /*we already have a placement for this cell no need to create any mappings*/
		}
		for (j=1 ; j <= maxVal ; j++){
			if (isValidm(b,(i/maxVal),(i%maxVal),j,blockw,blockh)){ /*from solver module- checks if j is a valid placement for this cell*/
				(m->cells)[i].num++; /*another variable for this cell*/
				pack(c,&cur,j); /*encodes in map data structure that value j is a valid candidate for this cell, updates cur to next node*/
			}
		}
		if ((m->cells)[i].num==0){ /*this cell in board did not give any variables even though it's still empty*/
			destroyMap(m);
			return NULL;
		}
		m->total+=(((m->cells)[i]).num);
	}
	return m;
}

/*
 * Returns how many variables this mapping holds.
 * Receives a pointer to the mapping.
 */
int GetNumVar(map* m){
	if (m==NULL){
		return 0;
	}
	return (m->total);
}

/*
 * Receives a pointer to the mapping and an index of a cell on the board.
 * Returns how many variables this cell contributed to the model.
 */
int GetNumCell (map* m, int index){
	return ((m->cells[index]).num);
}

/*
 * Returns the last index of a variable in this cell
 * Receives a pointer to the mapping and an index of a cell on the board.
 *
 */
int GetLastVar(map* m, int index){
	if ((m->cells[index]).num == 0){
		return -1; /*no variables for this cell*/
	}
	return ((m->cells[index]).index + (m->cells[index]).num -1);
}

/*
 * Returns the first index of a variable in this cell
 * Receives a pointer to the mapping and an index of a cell on the board.
 */
int GetFirstVar(map* m, int index){
	if ((m->cells[index]).num == 0){
			return -1; /*no variables for this cell*/
	}
	return (((m->cells)[index]).index);
}

/*
 *Returns the variable number in the model representing placement of "value" in cell "ind".
 *Receives a pointer to a mapping, index of a cell in the board and a value.
 *If there is no such variable, Returns -1
 */
int getMapping(map* m, int ind, int value){
	mnode* cur;
	int mapping;
	if (m == NULL || ((m->cells[ind]).num)==0){
		return -1;
	}
	cur = (m->cells[ind]).list;
	mapping = (m->cells[ind]).index;
	while (cur != NULL){
		if (cur->val == value){
			return mapping;
		}
		cur = cur->next;
		mapping++;
	}
	return -1;
}

/*
 *	Returns the index of the cell variable var belongs to.
 *	Receives a pointer to the map and the number of variable we wish to associate with a cell.
 *	Uses a variation on Binary search to economize.
 */
int getCell(map* m, int var){
	int up, down, mid;
	if (m==NULL || (var >= (m->total)) || (var <0)){
		return -1;
	}
	up = m->size-1;
	down = 0;
	while (up>=down){
		mid = ((up+down)/2);
		if (((m->cells)[mid]).index > var){ /*smallest variable in this cell is larger than var*/
			up = mid-1;
		}
		else{
			if (((((m->cells)[mid]).index + ((m->cells)[mid]).num) -1)  < var){ /*largest variable in this cell is smaller than var*/
				down = mid+1;
			}
			else{
				return mid;
			}
		}
	}
	return -1;
}

/*
 * Returns the value represented variable var.
 * If var doesn't belong to cell[index] returns -1
 */
int getVal(map* m, int ind, int var){
	mnode* cur;
	int get;
	if (m==NULL || ind<0 || ind >= m->size || var < 0 || var >= m->total || (m->cells[ind].num == 0)){
		return -1;
	}
	get = ((m->cells[ind]).index);
	cur = (m->cells[ind]).list;
	while ((get!=var) && (cur)!=NULL){
		cur=cur->next;
		get++;
	}
	return ((cur==NULL)? -1 : cur->val); /*if null return -1 otherwise we found the value*/
}

/*
 * Auxiliary function to insert another variable into the mapping.
 * This function receives a pointer to the map, pointer to the last updated node in the list of this cell (or to this cell if no
 * variable was updated yet) and the placement value this cell represents.
 *
 * The function extends the list in this cell of the map, adding a node representing the new variable.
 */
void pack(cell* c, mnode** cur,int j){
	if ((c->list)==NULL){
		assert((c->list = (mnode*) (calloc(1,sizeof(mnode))))!=NULL && "Memory allocation error");
		*cur = c->list;
		(*cur)->val = j;
		return;
	}
	assert((((*cur)->next) = (mnode*) (calloc(1,sizeof(mnode))))!=NULL && "Memory allocation error");
	*cur = (*cur)->next;
	(*cur)->val = j;
}

/*
 * frees all allocate data to this map and it's cells.
 */
void destroyMap(map* m){
	int i;
	for (i=0 ; i < m->size ; i++){
		destroyCell((&(m->cells[i]))->list);
	}
	free(m->cells);
	free(m);
}

void destroyCell(mnode* n){
	mnode* tmp;
	while (n!=NULL){
		tmp = n->next;
		free(n);
		n = tmp;
	}
}
