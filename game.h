/*
 * game.h
 *
 *	This header defines the data structure containing the game's board and the API to using and manipulating it.
 *
 *	The board is represented by a board structure containing Meta-data about the board's general state, the base of the data structure
 *	recording the moves history and pointers to two arrays:
 *	The first contains the cell values, and the second their abstract state (whether they are set or fixed).
 *
 *	As we want to encapsulate (as much as we can in C) this data structure, any other module performing complex changes to the board
 *	will receive an array representation of the board and only manipulate it through this API.
 *
 *	I choose to represent the board with 1d arrays in order to simplify handling this structure.
 *	The cells index is it's order in the board starting from 0 as the left top-most corner.
 *	The coordinate system can be easily translated to index from using the cordToInd function.
 *
 *  Created on: Feb 14, 2019
 *      Author: Edanz
 */

#ifndef GAME_H_
#define GAME_H_

#include "history.h"
#include "mode.h"

/*
 * defines a cell in a sudoku board
 */
typedef struct S_cell{
	unsigned int fixed:1; /*boolean bit field- is the cell pre-fixed*/
	unsigned int err:1; /*boolean bit field- is the cell erroneous*/
} cell;

/*
 * defines a the sudoku board and it's metadata
 */
typedef struct S_board{
	int* values; /*1d int array  the size of the board, containing the cell values*/
	cell* puzzle; /*1d cell array with the same size of values. puzzles[i] describes the state of cell[i] in values*/
	int blockW; /*block width*/
	int blockH; /*block height*/
	int free; /*number of un-assigned cells in board*/
	int nerr; /*number of erroneous cells*/
	int size; /*boards total size*/
	int solvable; /*remembers if board was validated in it's current state, 0=no knowlage, 1= found solvable, -1=found infeasble*/
	history hist; /*move history*/
} board;

/*
 * Creates a new board from supplied array and dimensions.
 * Assumes the array is atleast (blockh*blockw)*(blockh*blockw) long.
 *
 * Negative values are
 * This function ignores any illegal (non integer, exceeding blockw*blockh etc), or any values
 * that appear in indices larger than the board size.
 *
*/
board* createBoard(int* arr, int blockw, int blockh, mode m);

/*frees all space allocated to a board and it's cells*/
void destoryBoard(board* b);

/*
 * Outputs current board to array format to be used by auxiliary functions.
 * when (simple = 1) all values will appear as is in board (no marking fixed or errors).
 * when (simple = 0) fixed cells will be coded as negative, erroneous cells will be coded by adding (maxvlue + 1) their value.
 *
 * Output is a 1d array where cell's index is it's order in the board starting from 0 as the left top-most corner.
 *
 * Assumes supplied array is at least the board's size (can be checked with getSize(board) function).
 */
void toArray(board* b, int* arr, int simple);

/*
 * sets value of cell (x,y) / (arr[2],arr[1]) to arr[3].
 * the order of coardinates is switched with input because to match c's row major matrices.
 *
 * Assumes: value is valid, cell is not fixed and coordinated are legal.
 */
void set(board* b, int arr[]);

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
 */
int undo(board* b, int output[4]);

/*
 * Re-does the last action that was undone on board.
 * Same requirements and output as undo function.
 *
 */
int redo(board* b, int output[]);

/*
 * Returns board to the state it was just when created.
 */
void reset(board* b);

/*
 * Returns 1 if all of board's cell are in a valid state, 0 otherwise.
 * Not that this doesn't promise board is solvable.
 */
int allValid(board* b);

/*
 * checks if the game is over.
 * Returns 1 if all cells are full and valid, 0 otherwise.
 */
int gameOver(board* b);

/*
 * returns number of unassigned cells on board.
 */
int numFree(board* b);


/*
 * Returns 1 if all cells on board are empty, 0 otherwise
 */
int isEmpty(board* b);

/*
 * Returns 1 if board was validated since last change and found solvable, -1 if found unsolvable, 0 if a change was made.
 */
int isSolvable(board* b);

/*
 * Changes the solvable attribute of the board- if x is positive, board is solvable.
 * if x is negative, board is not solvable.
 * if x == 0, a change was made to the board.
 */
void setSolvable(board* b, int x);

/*
 * Returns the board's total size (number of cells).
 */
int getSize(board *b);

/*
 * updates blockdim[0] - with block width, blockdim[1] - height
*/
void getBlockDim(board *b, int blockdim[]);

/*
 * Returns maximum valid value to be filled in board
 */
int getMaxVal(board* b);

/*
 * translates coordinates to index in array received from toArray()
 */
int cordToInd(board *b, int cord[2]);

/*
 * Checks if cell in coordinates is fixed
 */
int isFixed(board* b, int cord[2]);

/*
 * Returns current value for cell in coordinates.
 */
int getCurVal(board* b, int cord[2]);

/*
 * Receives an array representation of a board and applies it to board.
 * Assumes supplied array is at least the boards size.
 *
 * (used to update board after moves such as autofill and generate)
 *
 */
void applyMatrix(board *b, int* arr);

#endif /* GAME_H_ */
