/*
 * solver.h
 *
 *	This module is in charge of the game's logic, or rule set.
 *	The functions here are used in all functions across the game to determine what makes a placement legal.
 *	A change in rule will only need to be applied in this module (and for some changes in the constraints function in the ILP).
 *
 *
 *  Created on: Feb 18, 2019
 *      Author: Edanz
 */

#ifndef SOLVER_H_
#define SOLVER_H_

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
 */
int num_solutions(int* b, int blockW, int blockH);

/*
 * Returns 1 if placing value val to cell (i+1,j+1) is valid. 0 otherwise.
 *
 * Receives array representation of board, i, j in row major 0 based coordinates, value and block sizes.
 * ****NOTE THIS FUNCTION RECIEVCES COORDINATES IN ROW MAJOR 0 BASED FORM****
 */
int isValidm(int* b, int i, int j, int val, int blockW, int blockH);

/*
 * Fills all obvious placements in board into b1, and returns number of cells filled.
 * Note that this function might result in an erroneous board (which means it's not solvable).
 *
 * Receives three int arrays: b1 representing a board, b2 with the same size, options- of at least maxVal size, and block sizes.
 *
 * Assumes all non empty cells of b2 contain the same values as b1.
 */
int autofill(int* b1, int* b2, int* options, int blockw, int blockh);

/*
 * Performs autofill moves on the board, until no obvious placements remain.
 * If the result is an erroneous board, returns 0.
 * otherwise returns 1.
 */
int fullAuto(int* b1, int blockw, int blockh);

/*
 * Finds all valid assignments to cell (index), stores them in options array and returns number of options.
 * Receives array representing board, cell index, array to store options output and block sizes.
 *
 * Assumes all array sizes fit the data (are according to block sizes)
 *
 */
int findOptions(int *b,int index,int *options, int blockw, int blockh);

/*
 * Translates cell number x in block to general index in board.
 * The reference point is the top leftmost cell in block.
 * Blocks are numbered from left to right top to bottom starting from 0
 */
int translateBlockIndex(int blockNum, int placeInBlock, int blockw, int blockh);

/*
 * Returns the block num of a the block containing the cell.
 * Recieces i- the row coordinate
 * 			j- the col coordinate
 * 			block dimensions
 */
int getBlockNum(int i, int j, int blockw, int blockh);

#endif /* SOLVER_H_ */
