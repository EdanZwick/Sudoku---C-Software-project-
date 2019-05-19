/*
 * mainAux.h
 *
 *  Created on: Feb 14, 2019
 *      Author: Edanz
 */

#ifndef MAINAUX_H_
#define MAINAUX_H_

#include "mode.h"
#include "game.h"

/*
 *
 * Prints errors if user command is invalid
 *		res[4] values denote:
 *		 -1 invalid command name
 *		 -2 command not available in mode
 *		 -3 Illegal number of arguments
*/
void handleParse(int* cmd, mode m);

/*
 * Sets a new game in solve mode, by loading a board from file "name"
 */
void handleSolve(board **b, char name[], mode *m, int mark);

/*
 * Sets a new game in edit mode, by loading a board from file "name" if it's non empty,
 * Or constructs a default empty board.
 */
void handleEdit(board **b, char name[], mode *m);

/*
 * Changes the mark errors variable accourding to input.
 */
void handleMark(int cmd[], int *mark);

/*
 * Prints game's board
 */
void handlePrint(board *b,int mark);

/*
 * Apples user set command or prints erorrs if not valid
 */
void handleSet(board *b, int *cmd, int mark, mode *m);

/*
 *Prints the needed output from the validate command.
 *Separated from the validate function bellow, as many actions include a validate step which doesn't require the same output
 */
void handleVali(board *b);

/*
 * Generates a board according to user input:
 *
 * Tries for MAX_GEN_ITERATIONS to fill x cells, and solve the board.
 * Applies new board if found or prints error otherwise.
 *
 */
void handleGen(board *b, int *cmd);

/*
 * Generates a board according to user input:
 *
 * Tries for MAX_GEN_ITERATIONS to fill x cells, and solve the board.
 * Applies new board if found or prints error otherwise.
 *
 */
void handleDo(board *b, int mark, int un,mode m);

/*
 * Saves board to file name according to format, or prints error if not permitable.
 */
void handleSave(board *b,mode m, char *name);

/*
 * prints a placement to cell cmd[2],cmd[1] that will keep the board solvable,
 * or prints an error if no such placement exists
 */
void handleHint(board *b, int *cmd);

/*
 * Prints number of possible solutions to the board, or error if none exist
 */
void handleNum(board *b);

/*
 * Filles every empty cell with only 1 possible value, or prints error if not possible.
 */
void handleAuto(board *b, int mark,mode *m);

#endif /* MAINAUX_H_ */
