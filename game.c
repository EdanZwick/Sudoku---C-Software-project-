/*
 * game.c
 *
 * This model is in charge of holding the representation of the game's board and provides all methods to edit it.
 * The Board data structure is defined in the header file.
 *
 *
 *  Created on: Feb 14, 2019
 *      Author: Edanz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "game.h"
#include "history.h"
#include "mode.h"
#include "solver.h"

int getMaxVal(board* b);
void simpleSet(board* b, int val, int index);
void markAll(board* b);
int cordToInd(board *b, int cord[2]);
void markErr(board* b, int i, int j);

/*
 * Creates a new board from supplied array and dimensions.
 * Assumes the array is at least (blockh*blockw)*(blockh*blockw) long.
 *
 * Negative values are
 * This function ignores any illegal (non integer, exceeding blockw*blockh etc), or any values
 * that appear in indices larger than the board size.
 *
*/
board* createBoard(int* arr, int blockw, int blockh, mode m){
	board *b;
	int i;
	cell *cur;
	assert((b = (board*) (malloc(sizeof(board))))!=NULL && "Memory allocation error");
	b->blockH = blockh;
	b->blockW = blockw;
	b->size = (blockh*blockw)*(blockh*blockw);
	b->free = b->size;
	b->nerr = 0;
	b->solvable = 0;
	createHistory(&(b->hist));
	assert((b->puzzle = (cell*) calloc(b->size , sizeof(cell)))!=NULL && "Memory allocation error");
	assert((b->values = (int*) calloc(b->size , sizeof(int)))!=NULL && "Memory allocation error");
	for (i = 0 ; i<b->size ; i++){ /*this loop updates all cells on board according to their value in input array*/
		cur = ((b->puzzle)+(i));
		b->values[i]= (abs(arr[i])<=(blockh*blockw)) ? abs(arr[i]) : 0 ; /*we won't allow illegal values*/
		cur->fixed = (arr[i]<0 && (b->values[i]!=0) && m!=edit);
		cur->err = 0;
		if (arr[i]!=0){
			b->free--;
		}
	}
	markAll(b); /*check all cells in the board if they are erroneous and up date the structure accordingly*/
	return b;
}

/*frees all space allocated to a board and it's cells*/
void destoryBoard(board* b){
	if (b==NULL){
		return;
	}
	free(b->puzzle);
	free(b->values);
	clearHistory(&(b->hist));
	free(b);
}

/*
 * Outputs current board to array format to be used by auxiliary functions.
 * when (simple = 1) all values will appear as is in board (no marking fixed or errors).
 * when (simple = 0) fixed cells will be coded as negative, erroneous cells will be coded by adding (maxvlue + 1) their value.
 *
 * Output is a 1d array where cell's index is it's order in the board starting from 0 as the left top-most corner.
 *
 * Assumes supplied array is at least the board's size (can be checked with getSize(board) function).
 */
void toArray(board* b, int* arr, int simple){
	int max = getMaxVal(b), i, size;
	size = b->size;
	memcpy(arr,b->values,size*sizeof(int));
	if (simple){
		return;
	}
	for (i=0; i < size ; i++){
		if (((b->puzzle)+i)->fixed){
			arr[i] *= -1;
		}
		else if (((b->puzzle)+i)->err){
			arr[i] += max + 1;
		}
	}
}


/*
 * sets value of cell (x,y) / (arr[2],arr[1]) to arr[3].
 * the order of coardinates is switched with input because to match c's row major matrices.
 *
 * Assumes: value is valid, cell is not fixed and coordinated are legal.
 */
void set(board* b, int arr[]){
	int index;
	index = cordToInd(b, arr+1);
	record(&(b->hist),arr[3],b->values[index],index,1); /*update history module*/
	simpleSet(b,arr[3],index); /*do the actual change in board*/
}

/*
 * Sets a value in board without updating the history module- used when we are sure of the value and cell is given in index
 * from (as opposed to coordinates).
 *
 * used for applying moves from history, multiple board updates etc.
 */
void simpleSet(board* b, int val, int index){
	int max;
	max = getMaxVal(b);
	if ((val) && (!(b->values[index]))){ /*z is not zero*/
		(b->free)--;
	}
	if ((b->values[index]) && !(val)){ /*we deleted the value in this cell*/
		(b->free)++;
	}
	(b->values[index]) = val;
	b->solvable = 0;
	markErr(b,(index/max),(index % max)); /*update all changes in validity of neighboring cells due to this placement*/
}

/*
 * Reverts the last action done on board.
 * Some changes to the board include more than one cell assignment, undo reverts them all.
 * If we reached the "dawn of history" the function does nothing and returns 0;
 *
 * Needs to recieve a 4 int array for full output.
 *
 * output[0] - col of cell that was changed
 * output[1] - row of cell that was changed
 * output[2] - old value
 * output[3] - new value
 *
 * if this change contains multiple placements we will just set the changed cell to -1 (main aux will print according to mode)
 *
 * Reminder: the output from past function:
 * 		arr[0] - value to set
 * 		arr[1] - index
 *
 */
int undo(board* b, int output[4]){
	int arr[2], id = curId(&(b->hist));
	past((&b->hist),arr); /*get details of last move, and correct the cur pointer in history*/
	if (arr[0]==-1){
		return 0;
	}
	output[0] = arr[1] % (b->blockH * b->blockW); /*pass what cell was changed*/
	output[1] = arr[1] / (b->blockH * b->blockW);
	output[2] = b->values[arr[1]]; /*pass what value this cell used to contain*/
	output[3] = arr[0]; /*pass the new value*/
	simpleSet(b,arr[0],arr[1]); /*do the change itself on the board*/
	if(curId(&(b->hist))==id){/*if the last move changed more than one cell, this output tells mainAux this is autofill/generate*/
		output[0] = -1;
	}
	while ((arr[0]!= -1) && curId(&(b->hist))==id){ /*We undo all changes until we reach a move with different id*/
		past((&b->hist),arr);
		simpleSet(b,arr[0],arr[1]);
	}
	return 1;
}

/*
 * Re-does the last action that was undone on board.
 * Same requirements and output as undo function.
 *
 */
int redo(board* b, int output[]){
	int arr[2], id = nextId(&(b->hist));
	future((&b->hist),arr);
	if (arr[0]==-1){
			return 0;
	}
	output[0] = arr[1] % (b->blockH * b->blockW); /*pass what cell was changed*/
	output[1] = arr[1] / (b->blockH * b->blockW); /*pass what cell was changed*/
	output[2] = b->values[arr[1]]; /*pass what value this cell used to contain*/
	output[3] = arr[0]; /*pass the new value*/
	simpleSet(b,arr[0],arr[1]);
	if(nextId(&(b->hist))==id){
			output[0] = -1;
	}
	while ((arr[0]!= -1) && nextId(&(b->hist))==id){
		future((&b->hist),arr);
		simpleSet(b,arr[0],arr[1]);
	}
	return 1;
}

/*
 * Returns board to the state it was just when created.
 */
void reset(board* b){
	int output[4];
	while (curId(&(b->hist))!=0){
		undo(b,output);
	}
}

/*
 * Returns 1 if all of board's cell are in a valid state, 0 otherwise.
 * Not that this doesn't promise board is solvable.
 */
int allValid(board* b){
	return (b->nerr==0);
}

/*
 * checks if the game is over.
 * Returns 1 if all cells are full and valid, 0 otherwise.
 */
int gameOver(board* b){
	return ((b->nerr==0)&&(b->free==0));
}

/*
 * returns number of unassigned cells on board.
 */
int numFree(board* b){
	return (b->free);
}

/*
 * Returns 1 if all cells on board are empty, 0 otherwise
 */
int isEmpty(board* b){
	return (b->free == (b->size));
}

/*
 * Returns 1 if board was validated since last change and found solvable, -1 if found unsolvable, 0 if a change was made.
 */
int isSolvable(board* b){
	return (b->solvable);
}

/*
 * Changes the solvable attribute of the board- if x is positive, board is solvable.
 * if x is negative, board is not solvable.
 * if x == 0, a change was made to the board.
 */
void setSolvable(board* b, int x){
	b->solvable = 0;
	if (x>0){
		b->solvable = 1;
	}
	else if (x<0){
		b->solvable = -1;
	}
}

/*
 * Returns the board's total size (number of cells).
 */
int getSize(board *b){
	return (b->size);
}

/*
 * updates blockdim[0] - with block width, blockdim[1] - height
*/
void getBlockDim(board *b, int blockdim[]){
	blockdim[0]=b->blockW;
	blockdim[1]=b->blockH;
}

/*
 * Returns maximum valid value to be filled in board
 */
int getMaxVal(board* b){
	return ((b->blockW)*(b->blockH));
}

/*
 * translates coordinates to index in array recieved from toArray()
 */
int cordToInd(board *b, int cord[2]){
	return ((cord[1]-1) * b->blockH * b->blockW) + cord[0] -1;
}

/*
 * Checks if cell in coordinates is fixed.
 */
int isFixed(board* b, int cord[2]){
	int index = cordToInd(b,cord);
	return ((b->puzzle)[index].fixed);
}

/*
 * Returns value of cell in coordinates
 */
int getCurVal(board* b, int cord[2]){
	int index = cordToInd(b,cord);
	return ((b->values)[index]);
}

/*
 * Receives an array representation of a board and applies it to board.
 * Assumes supplied array is at least the boards size.
 *
 * (used to update board after moves such as autofill and generate)
 *
 */
void applyMatrix(board *b, int* arr){
	int i, new = 1;
	clearTail(&(b->hist));
	for (i=0; i < b->size ; i++){
		if ((((b->values)[i]) != arr[i]) && !((b->puzzle + i)->fixed)){
			record(&(b->hist), arr[i], (b->values)[i], i,new);
			simpleSet(b, arr[i], i);
			new = 0;
		}
	}
}

/*
 * A change was made block "blockNum".
 * Check all cells and update valid bit according to the new situation
 */
void markBlock(board* b, int blockNum){
	int index , k, max = getMaxVal(b), err=0;
	cell* cur;
	for (k = 0 ; k<(b->blockH); k++){
		index = translateBlockIndex(blockNum, k, b->blockW, b->blockH);
		cur = (b->puzzle) + index;
		err = !isValidm(b->values, (index / max) , (index % max), b->values[index], b->blockW, b->blockH);
		b->nerr += err - (cur->err); /*updates counter according to weather the cell was already erroneous*/
		cur->err = err;
	}
}

/*
 * A change was made to row i.
 * Check all cells and update valid bit according to the new situation
 */
void markRow(board* b, int i){
	int max = getMaxVal(b), j, err=0;
	int index = i * max; /*first cell in row*/
	cell* cur;
	for (j = 0 ; j<max ; j++){
		cur = (b->puzzle) + index;
		err = !isValidm(b->values, i, j, b->values[index], b->blockW, b->blockH);
		b->nerr += err - (cur->err); /*updates counter according to weather the cell was already erroneous*/
		cur->err = err;
		index++;
	}
}

/*
 * A change was made to col j.
 * Check all cells and update valid bit according to the new situation
 */
void markCol(board* b, int j){
	int max = getMaxVal(b), i, err=0;
	int index = j;
	cell* cur;
	for (i = 0 ; i<max ; i++){
		cur = (b->puzzle) + index;
		err = !isValidm(b->values, i, j, b->values[index], b->blockW, b->blockH);
		b->nerr += err - (cur->err); /*updates counter according to weather the cell was already erroneous*/
		cur->err = err;
	index += max;
	}
}

/*
 * update all errors created or fixed by last placement
 * checks row, col and block even if error was already found, in order to update valid bits correctly
 */
void markErr(board* b, int i, int j){
	markRow(b,i);
	markCol(b,j);
	markBlock(b,getBlockNum(i,j,b->blockW,b->blockH));
}

/*
 * update all error bits in board.
 */
void markAll(board* b){
	int i, dim = b->blockW*b->blockH;
	for (i=0 ; i<dim ; i++){
		markRow(b,i);
		markCol(b,i);
		markBlock(b,i);
	}
}

