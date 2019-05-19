/*
 * ILP.h
 *
 * Provides API to solve Sudoku boards using ILP.
 * This module is dedicated to prime a board to be solved by ILP, pass it to GUROBI optimizer and process the library’s output.
 * Uses the map module to manage all variables passed to module.
 *
 *  Created on: Feb 21, 2019
 *      Author: Edanz
 */

#ifndef ILP_H_
#define ILP_H_

/*
 * Returns whether the board is solvable or not (1/0), and applies a solution on input array b (if apply==1).
 * ***apply==0 does not promise board will be unchanged, might edit board even if apply flag is turned off**
 *
 * Receives an array representing the board, block dimensions, flag whether to apply a solution if found, and a index cell that
 * denotes a single cell we wish to find a filling for.
 *
 * Assumes that board is in a valid state as supplied, block sizes are valid and cell is a legal index in board.
 *
 * The solution is done by the ILP function (which works with GUROBI).
 * This functions main role is to prime and initialize structures needed for the ILP.
 * To try and save work for the ILP, first assigns all cell which have only a single solution possible.
 *
 * If apply = 1
 * 		Fills the supplied array with the solution or returns 0 if impossible in current state.
 * If apply is negative, doesn't fill the whole array, but just the first cell with a hint for the cell in -(apply+1) index.
 * (this is used by the hint function and saves us the work of filling the entire array just to get 1 cell)
 *
 */
int solveB(int* b, int blockw, int blockh, int apply, int cell);

/*
 * Returns a legal assignment for cell index in board, or 0 if none exist.
 * Receives an array representation of the board, index of cell and block sizes.
 *
 * Assumes board is valid and index is legal.
 *
 */
int hint(int* b, int index, int blockw, int blockh);


#endif /* ILP_H_ */
