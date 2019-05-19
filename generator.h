/*
 * generator.h
 *
 *  Created on: Apr 20, 2019
 *      Author: Edanz
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

/*
 * Tries to generate a new game by using X random (but only legal) placements, using ILP to complete the board, then clear all but
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
 */
int generate(int* b, int x , int y, int blockw, int blockh, int numEmpty);

#endif /* GENERATOR_H_ */
