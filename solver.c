/*
 * solver.c
 *
 * Contains logic for legality of rules- when is placing a value to a cell legal?
 * A change in rules will only need to be modified here (and for some changes the create constraint in ILP).
 *
 * Uses auxiliary module, rec stack. A pseudo recursion stack to be used for the exhaustive backtracking function
 * that counts number of possible solutions for the board.
 *
 *
 *  Created on: Feb 15, 2019
 *      Author: Edanz
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "recStack.h"

int isValidm(int* b, int i, int j, int val, int blockW, int blockH);
int isValidRm(int* b, int i, int j, int val, int size);
int isValidCm(int* b, int i, int j, int val, int size);
int isValidBm(int* b, int i, int j, int val, int blockW, int blockH);
int next(int* b, int index, int len);
int findOptions(int *b,int index,int *options, int blockw, int blockh);
int isAllValid(int* b, int blockW, int blockH);

/*
 * Counts number of solutions possible for current board and Returns
 * number of different possible solutions for current board state (0 if none).
 *
 * Receives board in 1d array form and size parameters.
 * Assumes legal board size and matching block sizes.
 *
 * As required in the assignment, this function uses a pseudo recursive algorithm:
 * An auxiliary module "recStack" implements a "recursion stack".
 *
 * The procedure jumps between empty cells in array, trying all values from 1 to the maximal value permitted in board.
 * Once a legal placement is found, we assign it to the cell, push the index into a stack (helping us later to remember that this cell
 * was edited) and we move on trying to fill the next cells in board. Assigning a legal value to last cell in array means we have found
 * a solution to the board. if we reached a cell with no possible valid placements we backtrack to the last edited cell (by using the
 * stack to move back to it) and try a different placement.
 *
 */
int num_solutions(int* b, int blockW, int blockH){
	int counter = 0, index = -1, dim = (blockW)*(blockH), totalLen = (dim*dim), val;
	stack s;
	push(&s,-1); /*pad the stack with exit value of -1*/
	index = next(b,index,totalLen); /*looking from index -1 so we won't miss cell 0*/
	while (index>=0){
		if (index==totalLen){ /*reached end of board with a legal placement*/
			counter++;
			index = pop(&s);
			continue; /*skip rest of the iteration*/
		}
		val = b[index]+1; /*next value that was not checked for current cell*/
		while (val<=dim && (!(isValidm(b,(index/dim),(index%dim),val,blockW,blockH)))){ /*find next valid value for current cell*/
			val++;
		}
		if (val>dim){ /*no legal placement was found for cell*/
			b[index] = 0;
			index = pop(&s);
		}
		else{
			b[index] = val; /*place legal value found for cell*/
			push(&s,index);
			index = next(b,index,totalLen);
		}
	}
	return counter;
}

/*
 * Returns 1 if placing value val to cell (i+1,j+1) is valid. 0 otherwise.
 *
 * Receives array representation of board, i, j in row major 0 based coordinates, value and block sizes.
 * ****NOTE THIS FUNCTION RECIEVCES COORDINATES IN ROW MAJOR 0 BASED FORM****
 */
int isValidm(int* b, int i, int j, int val, int blockW, int blockH){
	if (val==0){/*always legal to delete a cell*/
		return 1;
	}
	return (isValidRm(b,i,j,val,blockW*blockH) && isValidCm(b,i,j,val,blockW*blockH) && isValidBm(b,i,j,val,blockW,blockH));
}

/*
 * Fills all obvious placements in board into b1, and returns number of cells filled.
 * Note that this function might result in an erroneous board (which means it's not solvable).
 *
 * Receives three int arrays: b1 representing a board, b2 with the same size, options- of at least maxVal size, and block sizes.
 *
 * Assumes all non empty cells of b2 contain the same values as b1.
 *
 * As this function might be called iteratively, it is kept lean and supplied with all the auxilary arrays,
 * to avoid repeated memory allocations (b2, options).
 *
 *
 */
int autofill(int* b1, int* b2, int* options, int blockw, int blockh){
	int i, num = 0, dim = blockw*blockh, size = (dim * dim);
	for (i=0; i<size ; i++){
		if (b1[i] == 0 && findOptions(b1,i,options, blockw, blockh)==1){ /*empty cell with 1 possible placment*/
			b2[i] = options[0];
			num++;
		}
	}
	for (i=0; i<size ; i++){
		if (b1[i]==0 && b2[i]!=0){
			b1[i] = b2[i];
		}
	}
	return num;
}

/*
 * Performs autofill moves on the board, until no obvious placements remain.
 * If the result is an erroneous board, returns 0.
 * otherwise returns 1.
 */
int fullAuto(int* b1, int blockw, int blockh){
	int *b2,*options;
	assert((b2 = (int*) calloc(blockw*blockh*blockw*blockh, sizeof(int)))!= NULL && "memory allocation error");
	assert((options = (int*) malloc(blockw* blockh * sizeof(int)))!= NULL && "memory allocation error");
	while (autofill(b1,b2,options,blockw,blockh)!=0);
	free(b2);
	free(options);
	return(isAllValid(b1, blockw, blockh));
}


/*
 * Finds all valid assignments to cell (index), stores them in options array and returns number of options.
 * Receives array representing board, cell index, array to store options output and block sizes.
 *
 * Assumes all array sizes fit the data (are according to block sizes)
 */
int findOptions(int *b,int index,int *options, int blockw, int blockh){
	int i, count = 0, max = blockw*blockh;
	for (i = 1 ; i<=max ; i++){
		if (isValidm(b,(index/max),(index%max),i,blockw,blockh)){
			options[count] = i;
			count++;
		}
	}
	return count;
}


/*
 * Translates cell number x in block to general index in board.
 * The reference point is the top leftmost cell in block.
 * Blocks are numbered from left to right top to bottom starting from 0
 */
int translateBlockIndex(int blockNum, int placeInBlock, int blockw, int blockh){
	int rows, cols, index, dim =blockw * blockh;
	index = (blockNum/blockh) * (blockh * dim); /*there are blockh blocks in a row, this gives us the leftmost index top row of block*/
	index += (blockNum % blockh) * blockw; /*this sets us to the top-leftmost index in block*/
	rows = placeInBlock / blockw;
	cols = placeInBlock % blockw;
	return (index + cols + (rows*dim)); /*index of desired cell is block's start index + number of cells in rows between them and col offset*/
}

/*
 * checks if placing val into (i,j) causes error in row.
 * Very similar to function in game module. The choice to duplicate here is because of subtle differences in requirements
 * between the two modules, and the choice to run all solver modules on an array instead of the board.
 * This path was chosen in order to avoid unwanted dependencies between the two modules.
 */
int isValidRm(int* b, int i, int j, int val, int size){
	int index = (i * size), k;
	for (k = 0 ; k < size ; k++){
		if ((b[index]==val) && (k!=j)){
			return 0;
		}
		index++;
	}
	return 1;
}

/*
 * checks if placing val into (i,j) causes error in column
 */
int isValidCm(int* b, int i, int j, int val, int size){
		int index = j, k;
		for (k = 0 ; k < size ; k++){
			if ((b[index]==val) && (k!=i)){
				return 0;
			}
			index += size;
		}
		return 1;
	}

/*
 * checks if placing val into (i,j) causes error in block
 */
int isValidBm(int* b, int i, int j, int val, int blockW, int blockH){
	int bi, bj, index , k, l, max = blockH*blockW, origI = i*max + j;
	bi = i - (i % (blockH)); /*index of first row in block*/
	bj = j - (j % (blockW)); /*index of first col in block*/
	for (k = 0 ; k<(blockH); k++){
		for (l = 0 ; l<(blockW) ; l++){
			index = (bi + k)*max + bj + l;
			if ((b[index]==val) && (index!=origI)){
				return 0;
			}
		}
	}
	return 1;
}

/*
 * Returns 1 if the whole board is valid
 * 0 otherwise
 */
int isAllValid(int* b, int blockW, int blockH){
	int i, dim =blockW * blockH, size = dim*dim;
	for (i=0; i<size ; i++){
		if (!isValidm(b,i/dim,i%dim,b[i],blockW,blockH)){
			return 0;
		}
	}
	return 1;
}

/*
 * Finds next non 0 cell in array.
 * If reaches the end of array without finding: returns len
 */
int next(int* b, int index, int len){
	index++;
	while (index<len && b[index]!=0){
		index++;
	}
	return (index);
}

/*
 * Returns the block num of a the block containing the cell.
 * Recieces i- the row coordinate
 * 			j- the col coordinate
 * 			block dimensions
 *
 */
int getBlockNum(int i, int j, int blockw, int blockh){
	int row, col;
	row = i / blockh; /*what row *of blocks* the block is*/
	col = j / blockw; /*what col *of blocks* the block is*/
	return ((row * blockh)+ col); /*there are blockh blocks in a row, note this is integer math so the operations don't cancel out*/
}


