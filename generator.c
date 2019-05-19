/*
 * generator.c
 *
 *
 * Provides the Generate function which generates a new game by using X random (but only legal) placements,
 * using ILP to complete the board, then clear all but Y random cells.
 *
 *
 *  Created on: Apr 20, 2019
 *      Author: Edanz
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "sizes.h"
#include "solver.h"
#include "ILP.h"


void findEmpty(int *b , int* empty ,int numEmpty, int size);
int fillRanWithRan(int *b, int* empty, int blockw, int blockh, int left, int* options);
void swap (int* empty, int left, int index);
void restoreEmpty(int* b,int* empty, int left, int numEmpty);
void ClearButRanY(int* b,int y,int size);

/*
 * Tries to generates a new game by using X random (but only legal) placements, using ILP to complete the board, then clear all but
 * Y random cells.
 *
 * Receives an int array b, which is assumed to be at least the board's size, and returns it with a generated board if successful.
 * If during MAX_GEN_ITERATIONS (set to 1000 as default) iteration, every attempt at placing x random cells led to an infeasible
 * Board (whether discovered infeasible during the placement process or ILP)- declares failure and returns 0.
 *
 * Also recieves: numEmpty- the number of empty cells in the board, the paramaters x (which is assumed to be a non-negative number, smaller
 * than numEmpty) and y (which is assumed to be non-negative and smaller than the board's size), and the block
 * dimentions of the board.
 *
 * Assumes the board is in legal state, has a solution, and contains enough empty cells.
 *
 * To avoid situations where we need to repeatedly pick random cells, we keep track of all empty cells in the board in "empty" array.
 * If we assign a value to a cell we move it to the end of the array, and maintain the index separating cells which we already
 * filled and the ones that are still empty.
 * This also lets us keep track on exactly what cells we need to restore between iterations (saving us the time of going *All* the empty
 * cells originally found and emptying them).
 *
 * General algorithm:
 * 		1. find all empty cells in board and store their indices in "empty" array.
 * 		2. for X times: randomly pick a cell in empty array between the places 0 and stillEmpty.
 * 						find all possible values for this cell:
 * 							if exist, pick one at random and place in this cell.
 * 								move index of this cell in the board to the end of empty array and decrement stillEmpty.
 * 								(this will make sure every randomization over the empty array will yield a cell that has not been
 * 								filled already).
 * 							if no placements exists, finish this iteration by restoring the board to it's original state.
 * 		3. if we successful placed x cells, we try and solve the board:
 * 				if successful- break from the iterations and store the solved board.
 * 				if unsucsessful - clear all the cells in the empty array (as the SolveB function might soil the board), and continue
 * 				iterations.
 * 		4. if a solution is found, clear all but y cells in the board and declare success.
 *
 */
int generate(int* b, int x , int y, int blockw, int blockh, int numEmpty){
	int finish = 0, iterations=0, left, *empty, size = blockw*blockw*blockh*blockh, stillEmpty, *options;
	srand(time(0));
	/*build an array holding all indexes of empty cells*/
	assert((empty = (int*)calloc(numEmpty,sizeof(int)))!=NULL);
	findEmpty(b, empty ,numEmpty, size);
	assert((options = (int*) malloc((blockw*blockh*sizeof(int))))!=NULL);
	/*try for MAX_GEN_ITERATIONS (defined in sizes.h):*/
	while ((iterations < MAX_GEN_ITERATIONS) && (!finish)){
		left = x;
		stillEmpty = numEmpty;
		/*randomly choose x cells and assign a random possible value*/
		while (left){
			if (!(fillRanWithRan(b,empty,blockw,blockh,stillEmpty,options))){ /*some cell did not have any legal placement*/
				stillEmpty--;
				break; /*breaks only out of the inner loop, board will restore and we will try another randomization*/
			}
			left--;
			stillEmpty--;
		}
		/*if we successfully assigned x cells try and solve board*/
		if (!left){
				if (solveB(b,blockw,blockh,1,-1)){
					finish = 1;
					break; /*breaks out of the loop without restoring the board. this is the solution we return*/
				}
				else{
					stillEmpty = 0; /*As solveB function might edit the board (full auto), we need to make sure the whole board is restored*/
				}
		}
		restoreEmpty(b,empty,stillEmpty,numEmpty);
		iterations++;
	}
	free(options);
	free(empty);
	if (!finish){
		return 0;
	}
	/*erase all but y cells from the board*/
	ClearButRanY(b,y,size);
	return 1;
}

/*
 * Fills empty array with indices of all empty cells in board.
 */
void findEmpty(int *b , int* empty, int numEmpty, int size){
	int count = 0, i = 0;
	while ((count < numEmpty) && i<size){
		if (b[i]==0){
			empty[count]=i;
			count++;
		}
		i++;
	}
}

/*
 * chooses a random cell and fills it with a random *valid* number.
 * If no values are allowed for a chosen cell, the function returns 0.
 *
 * This function receives an array representing the board, an array containing all the indices of empty values in the board,
 * board dimensions, number of empty cells and options array in which to store the options for the cell.
 *
 * Assumes board is in legal format, empty array at least stillempty long, and options array is at least blockw*blockh long.
 *
 * I choose to supply the options array externally as this function might be called iteratively, this will allow us to allocate
 * the array only once thus saving considerable overhead.
 *
 * The algorithm:
 * 1. randomize a value between 0 and number of left cells- this is the empty cell we shall fill.
 * 2. find what legal filling options this cell has- store them in an array.
 * 3. randomize a value between 0 and number of options cell has- index
 * 4. place the value that is in the "index" cell of options array in the board.
 *
 *
 * As throughout the process we might want to restore the empty cells, we keep their indices in the end of the empties
 * array. after filling a cell, this function moves it's index to the end part of the empties array so it won't be considered again.
 *
 */
int fillRanWithRan(int *b, int* empty, int blockw, int blockh, int stillEmpty, int* options){
	int index=0, num=0, pick=0, tmp;
	tmp = rand() % stillEmpty;
	index = empty[tmp]; /*index is now an index of a random empty cell in the *board* itself */
	swap(empty,stillEmpty,tmp); /*need to move the index of the cell we will fill to the end of empty array so we won't choose it again*/
	num = findOptions(b,index,options,blockw,blockh);
	if (num == 0){
		return 0;
	}
	pick = rand() % (num);
	b[index] = options[pick];
	return 1;
}

/*
 * After an empty cell has been filled, we move it to the end of array so we won't keep considering it in future randomizations.
 * Left is the number of empty cells currently in array. stillEmpty-1 is the index of last empty cell in empty array.
 */
void swap (int* empty, int stillEmpty, int index){
	int tmp=0;
	tmp = empty[stillEmpty - 1];
	empty[stillEmpty - 1] = empty[index];
	empty[index] = tmp;
}

/*
 * If we made an unsuccessful attempt at filling x random cells, we need to clear all cells we already filled before next try
 * The indices of these cells were moved to the back of empty array.
 * The fact that the indices are now out of order in empty array does not affect us (as we are choosing from it randomly).
 */
void restoreEmpty(int* b,int* empty, int lastNotEmpty, int numEmpty){
	lastNotEmpty = (lastNotEmpty<0) ? 0 : lastNotEmpty; /*make sure we won't get a negative index*/
	for (;  lastNotEmpty < numEmpty ; lastNotEmpty++){
		b[empty[lastNotEmpty]]=0;
	}
}

/*
 * Randomly chooses y cells out of a full board to keep. clears all cells but them.
 * To try and reduce number of randomization cycles we will "invest" in creating an index array and limit our randomization to indices
 * that were not picked before.
 */
void ClearButRanY(int* b,int y,int size){
	int num = size, *choice, i = 0, toClear = size - y, tmp;
	assert((choice = (int*) calloc(size,sizeof(int)))!=NULL);
	for (i = 0; i < size; i++){/*fill array with indeces*/
		choice[i] = i;
	}
	for (i = 0 ; i < toClear ; i++){
		tmp = rand() % num;
		b[choice[tmp]] = 0;
		swap(choice,num,tmp);
		num--;
	}
	free(choice);
}

